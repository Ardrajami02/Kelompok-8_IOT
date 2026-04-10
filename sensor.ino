
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

const char* ssid     = "HOTSPOT@UPNJATIM.AC.ID";
const char* password = "belanegara";

// Hardware Pins
#define DHTPIN 0      // D3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Web Server
ESP8266WebServer server(80);

// ================= ROOT (WEB UI) =================
void handleRoot() {
  digitalWrite(LED_BUILTIN, LOW);

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Validasi jika sensor gagal baca
  if (isnan(t) || isnan(h)) {
    t = 0;
    h = 0;
  }

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<meta http-equiv='refresh' content='10'>
<title>Kelompok 8 - FARHANNNNNN</title>

<style>
    body {
        font-family: Arial;
        text-align: center;
        background: linear-gradient(to right, #3d36ff, #f5057d);
        color: white;
        margin: 0;
        padding: 0;
    }
    .container {
        margin-top: 50px;
        background: rgba(54,168,256,3);
        padding: 20px;
        border-radius: 50px;
        width: 300px;
        margin-left: auto;
        margin-right: auto;
        box-shadow: 0px 4px 15px rgba(0,0,0,0.3);
    }
    h1 { margin-bottom: 5px;  margin-top: 5px; }
    h3 { margin-top: 0; font-weight: normal; }
    .data {
        font-size: 24px;
        margin: 15px 0;
    }
    .footer {
        margin-top: 20px;
        font-size: 12px;
        opacity: 0.8;
    }
</style>

</head>

<body>

<div class="container">
    <h1>SENSOR E FARHAN</h1>
    <h3>Kelompok 8</h3>

    <div class="data">
        Suhu: <b>%TEMP% °C</b>
    </div>

    <div class="data">
        Kelembapan: <b>%HUM% %</b>
    </div>

</div>

</body>
</html>
)rawliteral";

  html.replace("%TEMP%", String(t));
  html.replace("%HUM%", String(h));

  server.send(200, "text/html", html);

  digitalWrite(LED_BUILTIN, HIGH);
}

// ================= JSON =================
void handleRootJSON() {
  digitalWrite(LED_BUILTIN, LOW);

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    t = 0;
    h = 0;
  }

  String json = "{";
  json += "\"temperature\": " + String(t) + ",";
  json += "\"humidity\": " + String(h);
  json += "}";

  server.send(200, "application/json", json);

  digitalWrite(LED_BUILTIN, HIGH);
}

// ================= SETUP =================
void setup() {
  Serial.println("JAJAL\n");
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // ROUTING
  server.on("/", handleRoot);
  server.on("/data", handleRootJSON); // ✅ tambahan penting

  server.begin();

  // OLED tampilkan IP
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("IP Address:");

  display.setTextSize(2);
  display.setCursor(0,20);
  display.println(WiFi.localIP());
  display.display();

  Serial.println(WiFi.localIP());
}

// ================= LOOP =================
void loop() {
  server.handleClient();
}