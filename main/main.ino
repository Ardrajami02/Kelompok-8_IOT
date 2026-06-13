// Name: main.ino
// Desc: Files arduino eas matkul iot
// Auth: KillingWhales et all

#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>

// CONNECTION
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define BUILTINLEDPINOUT 2
#define DHTPIN 27 //HUM gpio27dht
#define MAX4466_PIN 34 //SOUND gpio34
#define BH1750SDA_PIN 21 //LIGHT gpio21
#define BH1750SCL_PIN 22 //LIGHT gpio22

// // MACROS // //

// Fungsi untuk blink dengan cepat berdasarkan amount
#define gettingToBlinky(amount, spd) for(int i = 0;i < amount;i++){\
    digitalWrite(BUILTINLEDPINOUT, HIGH);\
    delay(spd);\
    digitalWrite(BUILTINLEDPINOUT, LOW);\
    delay(spd);\
}

// Macros Expansion untuk init semua sensor
#define initAllLib(dht_loc) dht_loc.begin(); delay(3000); Wire.begin(BH1750SDA_PIN, BH1750SCL_PIN)

// // GLOBAL VAR // //

const char* GC_WINAME[] = {"Kayasip "}; // tambahkan wifi terus, beserta dengan pwnya
const char* GC_WIPASSWORD[] = {"raffi123"};

IPAddress staticIP(192, 168, 1, 150);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// nanti implementasinya hanya for (int i = 0; i < sizeof(GC_WINAME) / sizeof(GC_WINAME[0]);i++){ GC_WINAME[i]; GC_WIPASSWORD[i]; }

DHT hum(DHTPIN, DHT22);
BH1750 light;

WebServer server(80);
// // FORWARD DECLARATION // //
void handleScream();


// // FUNCTION // //


void
setup() {
  pinMode(BUILTINLEDPINOUT, OUTPUT); // LED BLINK / gettingToBlinky
  Serial.begin(9600);
  // initAllLib(hum);
  hum.begin(); delay(3000); Wire.begin(BH1750SDA_PIN, BH1750SCL_PIN); // init singline
  if (light.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 OK");
  } else {
    Serial.println("BH1750 FAILED");
  }


  WiFi.mode(WIFI_STA);
  WiFi.config(staticIP, gateway, subnet);

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    for (int i = 0; i < sizeof(GC_WINAME) / sizeof(GC_WINAME[0]); ++i){
      WiFi.begin(GC_WINAME[i], GC_WIPASSWORD[i]);
      if (WiFi.status() == WL_CONNECTED) break;
    }
    delay(500);
    timeout++;
  }
  delay(3000);

  Serial.println(WiFi.localIP());

  // ROUTING
  server.on("/", handleScream);
  server.begin();
}

void
loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    // Serial.println(WiFi.localIP());
    gettingToBlinky(2, 100);
  }
  delay(10);
}

void handleScream(){
  int micval = analogRead(MAX4466_PIN);
  float humval = hum.readHumidity();
  float tempval = hum.readTemperature();
  float luxval = light.readLightLevel();
  if (isnan(tempval) || isnan(humval)){
      tempval = 0;
      humval=0;
      }
  if (isnan(luxval))
      luxval = 0;

  String response =
      String(tempval) + "," +
      String(humval) + "," +
      String(luxval) + "," +
      String(micval);

  server.send(
      200,
      "text/plain",
      response);
}


// void handleRoot() {
//
//   // float t = dht.readTemperature();
//   // float h = dht.readHumidity();
//   //
//   // // Validasi jika sensor gagal baca
//   // if (isnan(t) || isnan(h)) {
//   //   t = 0;
//   //   h = 0;
//   // }
//
//   int micval = analogRead(MAX4466_PIN);
//   float humval = hum.readHumidity();
//   float tempval = hum.readTemperature();
//   float luxval = light.readLightLevel();
//
//   if (isnan(micval)) micval = 0;
//   if (isnan(luxval)) luxval = 0.0;
//   if (isnan(tempval) || isnan(humval)) {tempval=0.0; humval=0.0;};
//
//
//   String html = R"rawliteral(
//   <!DOCTYPE html>
//   <html lang="id">
//   <head>
//     <meta charset="UTF-8" />
//     <meta name="viewport" content="width=device-width, initial-scale=1.0" />
//     <meta http-equiv="refresh" content="5" />
//     <title>Kelompok 8 – IoT Peternakan</title>
//
//     <!-- Google Fonts: Space Grotesk for techy-farmtech feel -->
//     <link rel="preconnect" href="https://fonts.googleapis.com" />
//     <link href="https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@400;600;700;800&display=swap" rel="stylesheet" />
//
//     <style>
//       /* ─── Catppuccin Mocha palette ──────────────────────────── */
//       :root {
//         --base:   #1e1e2e;
//         --mantle: #181825;
//         --surface0: #313244;
//         --surface1: #45475a;
//         --overlay0: #6c7086;
//         --text:   #cdd6f4;          /* card-1 border / main text */
//         --subtext:#a6adc8;
//
//         --lavender:#b4befe;         /* humidity / card-2 */
//         --pink:    #f38ba8;         /* temperature / card-1 */
//         --yellow:  #f9e2af;         /* light / card-3 */
//         --green:   #a6e3a1;         /* sound / card-4 */
//         --blue:    #89b4fa;
//         --mauve:   #cba6f7;
//
//         --radius-card: 18px;
//         --radius-badge: 12px;
//       }
//
//       *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
//
//       body {
//         font-family: 'Space Grotesk', Arial, sans-serif;
//         background-color: var(--base);
//         color: var(--text);
//         min-height: 100vh;
//         padding: 20px 16px 32px;
//       }
//
//       /* ─── HEADER ────────────────────────────────────────────── */
//       .header {
//         display: flex;
//         justify-content: space-between;
//         align-items: flex-start;
//         margin-bottom: 28px;
//         gap: 12px;
//       }
//       .header-left h1 {
//         font-size: clamp(1.5rem, 5vw, 2.6rem);
//         font-weight: 800;
//         line-height: 1.1;
//         letter-spacing: -0.02em;
//         text-transform: uppercase;
//       }
//       .header-left .subtitle {
//         margin-top: 6px;
//         font-size: clamp(0.65rem, 2vw, 0.8rem);
//         color: var(--subtext);
//         text-transform: uppercase;
//         letter-spacing: 0.08em;
//         max-width: 320px;
//       }
//       .header-right {
//         display: flex;
//         gap: 12px;
//         align-items: center;
//         flex-shrink: 0;
//       }
//       .icon-btn {
//         background: var(--surface0);
//         border: none;
//         border-radius: 10px;
//         width: 40px; height: 40px;
//         display: flex; align-items: center; justify-content: center;
//         cursor: pointer;
//         color: var(--text);
//         transition: background 0.2s;
//       }
//       .icon-btn:hover { background: var(--surface1); }
//       .icon-btn svg { width: 18px; height: 18px; }
//
//       /* ─── SENSOR CARDS GRID ─────────────────────────────────── */
//       .cards-grid {
//         display: grid;
//         grid-template-columns: repeat(2, 1fr);
//         gap: 14px;
//         margin-bottom: 14px;
//       }
//
//       @media (min-width: 700px) {
//         .cards-grid { grid-template-columns: repeat(4, 1fr); }
//       }
//
//       .sensor-card {
//         background: var(--mantle);
//         border-radius: var(--radius-card);
//         border: 2.5px solid;
//         padding: 16px 14px 14px;
//         position: relative;
//         overflow: hidden;
//         transition: transform 0.15s;
//       }
//       .sensor-card:hover { transform: translateY(-2px); }
//
//       .sensor-card.temp  { border-color: var(--pink);    }
//       .sensor-card.hum   { border-color: var(--lavender);}
//       .sensor-card.light { border-color: var(--yellow);  }
//       .sensor-card.sound { border-color: var(--green);   }
//
//       .card-header {
//         display: flex;
//         align-items: center;
//         gap: 8px;
//         margin-bottom: 8px;
//       }
//       .card-icon {
//         width: 28px; height: 28px;
//         flex-shrink: 0;
//       }
//       .card-label {
//         font-size: clamp(0.6rem, 1.8vw, 0.75rem);
//         font-weight: 700;
//         text-transform: uppercase;
//         letter-spacing: 0.1em;
//         color: var(--subtext);
//       }
//       .card-value {
//         font-size: clamp(2.2rem, 8vw, 3.4rem);
//         font-weight: 800;
//         line-height: 1;
//         letter-spacing: -0.03em;
//       }
//       .card-unit {
//         font-size: clamp(0.6rem, 1.8vw, 0.75rem);
//         font-weight: 600;
//         color: var(--subtext);
//         text-transform: uppercase;
//         margin-top: 2px;
//       }
//
//       .sensor-card.temp  .card-value { color: var(--pink);    }
//       .sensor-card.hum   .card-value { color: var(--lavender);}
//       .sensor-card.light .card-value { color: var(--yellow);  }
//       .sensor-card.sound .card-value { color: var(--green);   }
//
//       /* ─── STATUS BADGES GRID ────────────────────────────────── */
//       .status-grid {
//         display: grid;
//         grid-template-columns: repeat(2, 1fr);
//         gap: 14px;
//       }
//
//       @media (min-width: 700px) {
//         .status-grid { grid-template-columns: repeat(4, 1fr); }
//       }
//       .status-badge {
//         border-radius: var(--radius-badge);
//         padding: 14px 16px;
//         display: flex;
//         align-items: center;
//         gap: 10px;
//         font-size: clamp(0.75rem, 2.5vw, 1rem);
//         font-weight: 700;
//         text-transform: uppercase;
//         letter-spacing: 0.06em;
//       }
//       .status-badge svg { width: 22px; height: 22px; flex-shrink: 0; }
//
//       .status-badge.temp  { background: rgba(243,139,168,0.15); color: var(--pink);    }
//       .status-badge.hum   { background: rgba(180,190,254,0.15); color: var(--lavender);}
//       .status-badge.light { background: rgba(249,226,175,0.15); color: var(--yellow);  }
//       .status-badge.sound { background: rgba(166,227,161,0.15); color: var(--green);   }
//
//       /* ─── FOOTER ────────────────────────────────────────────── */
//       .footer {
//         margin-top: 24px;
//         text-align: center;
//         font-size: 0.7rem;
//         color: var(--overlay0);
//         letter-spacing: 0.06em;
//       }
//       .footer span { color: var(--subtext); }
//
//       /* ─── LIVE PULSE DOT ────────────────────────────────────── */
//       .live-dot {
//         display: inline-block;
//         width: 8px; height: 8px;
//         border-radius: 50%;
//         background: var(--green);
//         margin-right: 6px;
//         animation: pulse 1.6s ease-in-out infinite;
//       }
//       @keyframes pulse {
//         0%, 100% { opacity: 1; transform: scale(1); }
//         50%       { opacity: 0.4; transform: scale(0.7); }
//       }
//
//       /* ─── DIVIDER ───────────────────────────────────────────── */
//       .divider {
//         height: 1px;
//         background: var(--surface0);
//         margin: 14px 0;
//       }
//     </style>
//   </head>
//   <body>
//
//   <!-- ── HEADER ──────────────────────────────────────────────── -->
//   <div class="header">
//     <div class="header-left">
//       <h1>Kelompok 8<br/>IoT – Peternakan</h1>
//       <p class="subtitle">Monitoring sensor kandang secara real-time.<br/>Data diperbarui setiap 10 detik.</p>
//     </div>
//     <div class="header-right">
//       <!-- Download icon -->
//       <button class="icon-btn" title="Download data" onclick="downloadData()">
//         <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//           <path d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/>
//         </svg>
//       </button>
//       <!-- Settings icon -->
//       <button class="icon-btn" title="Pengaturan threshold" onclick="toggleSettings()">
//         <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//           <circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 00.33 1.82l.06.06a2 2 0 010 2.83 2 2 0 01-2.83 0l-.06-.06a1.65 1.65 0 00-1.82-.33 1.65 1.65 0 00-1 1.51V21a2 2 0 01-4 0v-.09A1.65 1.65 0 009 19.4a1.65 1.65 0 00-1.82.33l-.06.06a2 2 0 01-2.83-2.83l.06-.06A1.65 1.65 0 004.68 15a1.65 1.65 0 00-1.51-1H3a2 2 0 010-4h.09A1.65 1.65 0 004.6 9a1.65 1.65 0 00-.33-1.82l-.06-.06a2 2 0 012.83-2.83l.06.06A1.65 1.65 0 009 4.68a1.65 1.65 0 001-1.51V3a2 2 0 014 0v.09a1.65 1.65 0 001 1.51 1.65 1.65 0 001.82-.33l.06-.06a2 2 0 012.83 2.83l-.06.06A1.65 1.65 0 0019.4 9a1.65 1.65 0 001.51 1H21a2 2 0 010 4h-.09a1.65 1.65 0 00-1.51 1z"/>
//         </svg>
//       </button>
//     </div>
//   </div>
//
//   <!-- ── SENSOR VALUE CARDS ────────────────────────────────────── -->
//   <div class="cards-grid">
//
//     <!-- Temperature -->
//     <div class="sensor-card temp">
//       <div class="card-header">
//         <!-- thermometer icon -->
//         <svg class="card-icon" viewBox="0 0 24 24" fill="none" stroke="#f38ba8" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
//           <path d="M14 14.76V3.5a2.5 2.5 0 00-5 0v11.26a4.5 4.5 0 105 0z"/>
//         </svg>
//         <span class="card-label">Suhu</span>
//       </div>
//       <div class="card-value">%TEMPERATURE%</div>
//       <div class="card-unit">°C · Celsius</div>
//     </div>
//
//     <!-- Humidity -->
//     <div class="sensor-card hum">
//       <div class="card-header">
//         <!-- droplet icon -->
//         <svg class="card-icon" viewBox="0 0 24 24" fill="none" stroke="#b4befe" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
//           <path d="M12 2.69l5.66 5.66a8 8 0 11-11.31 0z"/>
//         </svg>
//         <span class="card-label">Kelembapan</span>
//       </div>
//       <div class="card-value">%HUMIDITY%</div>
//       <div class="card-unit">% · Humidity</div>
//     </div>
//
//     <!-- Light -->
//     <div class="sensor-card light">
//       <div class="card-header">
//         <!-- bulb icon -->
//         <svg class="card-icon" viewBox="0 0 24 24" fill="none" stroke="#f9e2af" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
//           <line x1="9" y1="18" x2="15" y2="18"/><line x1="10" y1="22" x2="14" y2="22"/>
//           <path d="M15.09 14c.18-.98.65-1.74 1.41-2.5A4.65 4.65 0 0018 8 6 6 0 006 8c0 1 .23 2.23 1.5 3.5A4.61 4.61 0 018.91 14"/>
//         </svg>
//         <span class="card-label">Cahaya</span>
//       </div>
//       <div class="card-value">%LIGHTMETER%</div>
//       <div class="card-unit">lux</div>
//     </div>
//
//     <!-- Sound -->
//     <div class="sensor-card sound">
//       <div class="card-header">
//         <!-- volume icon -->
//         <svg class="card-icon" viewBox="0 0 24 24" fill="none" stroke="#a6e3a1" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
//           <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"/><path d="M15.54 8.46a5 5 0 010 7.07"/><path d="M19.07 4.93a10 10 0 010 14.14"/>
//         </svg>
//         <span class="card-label">Suara</span>
//       </div>
//       <div class="card-value">%SOUND%</div>
//       <div class="card-unit">dB</div>
//     </div>
//
//   </div>
//
//   <div class="divider"></div>
//
//   <!-- ── STATUS BADGES ─────────────────────────────────────────── -->
//   <div class="status-grid">
//
//     <div class="status-badge temp">
//       <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//         <path d="M14 14.76V3.5a2.5 2.5 0 00-5 0v11.26a4.5 4.5 0 105 0z"/>
//       </svg>
//       <span id="status-temp">Memuat…</span>
//     </div>
//
//     <div class="status-badge hum">
//       <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//         <path d="M12 2.69l5.66 5.66a8 8 0 11-11.31 0z"/>
//       </svg>
//       <span id="status-hum">Memuat…</span>
//     </div>
//
//     <div class="status-badge light">
//       <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//         <line x1="9" y1="18" x2="15" y2="18"/><line x1="10" y1="22" x2="14" y2="22"/>
//         <path d="M15.09 14c.18-.98.65-1.74 1.41-2.5A4.65 4.65 0 0018 8 6 6 0 006 8c0 1 .23 2.23 1.5 3.5A4.61 4.61 0 018.91 14"/>
//       </svg>
//       <span id="status-light">Memuat…</span>
//     </div>
//
//     <div class="status-badge sound">
//       <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round">
//         <polygon points="11 5 6 9 2 9 2 15 6 15 11 19 11 5"/>
//         <path d="M15.54 8.46a5 5 0 010 7.07"/>
//       </svg>
//       <span id="status-sound">Memuat…</span>
//     </div>
//
//   </div>
//
//   <!-- ── FOOTER ─────────────────────────────────────────────────── -->
//   <div class="footer">
//     <span class="live-dot"></span>
//     Live · diperbarui setiap 10 detik &nbsp;|&nbsp;
//     Terakhir: <span id="last-update">–</span>
//   </div>
//
//   <!-- ═══════════════════════════════════════════════════════════ -->
//   <!--  THRESHOLD SETTINGS PANEL (hidden by default)              -->
//   <!-- ═══════════════════════════════════════════════════════════ -->
//   <div id="settings-panel" style="
//     display:none;
//     position:fixed; top:0; left:0; right:0; bottom:0;
//     background:rgba(0,0,0,0.6);
//     z-index:100;
//     align-items:center;
//     justify-content:center;
//   ">
//     <div style="
//       background:var(--mantle);
//       border-radius:20px;
//       padding:28px 24px;
//       width:min(92vw, 420px);
//       border:1.5px solid var(--surface1);
//     ">
//       <h2 style="font-size:1.1rem;font-weight:800;text-transform:uppercase;letter-spacing:.08em;margin-bottom:20px;">
//         ⚙️ Atur Threshold Sensor
//       </h2>
//
//       <div id="threshold-form"></div>
//
//       <div style="display:flex;gap:10px;margin-top:20px;">
//         <button onclick="saveThresholds()" style="
//           flex:1;background:var(--lavender);color:var(--base);
//           border:none;border-radius:10px;padding:12px;
//           font-family:inherit;font-weight:700;font-size:.9rem;cursor:pointer;
//         ">Simpan</button>
//         <button onclick="toggleSettings()" style="
//           flex:1;background:var(--surface0);color:var(--text);
//           border:none;border-radius:10px;padding:12px;
//           font-family:inherit;font-weight:700;font-size:.9rem;cursor:pointer;
//         ">Tutup</button>
//       </div>
//     </div>
//   </div>
//
//   </body>
//   </html>
//   )rawliteral";
//
//
//   html.replace("%TEMPERATURE%", String(tempval));
//   html.replace("%HUMIDITY%", String(humval));
//   html.replace("%LIGHTMETER%", String(luxval));
//   html.replace("%SOUND%", String(micval));
//
//   server.send(200, "text/html", html);
//
// }

// ================= JSON =================
// void handleRootJSON() {
//   digitalWrite(LED_BUILTIN, LOW);
//
//   // float t = dht.readTemperature();
//   // float h = dht.readHumidity();
//   //
//   // if (isnan(t) || isnan(h)) {
//   //   t = 0;
//   //   h = 0;
//   // }
//
//   String json = "{";
//   json += "\"temperature\": " + String(t) + ",";
//   json += "\"humidity\": " + String(h);
//   json += "}";
//
//   server.send(200, "application/json", json);
//
//   digitalWrite(LED_BUILTIN, HIGH);
// }
