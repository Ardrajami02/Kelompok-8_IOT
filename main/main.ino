// Name: main.ino
// Desc: Files arduino eas matkul iot
// Auth: KillingWhales et all

#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>

// CONNECTION
#include <WiFi.h>
#include <WebServer.h>

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

const char* GC_WINAME[] = {"HOTSPOT@UPNJATIM.AC.ID", "Kayasip "}; // tambahkan wifi terus, beserta dengan pwnya
const char* GC_WIPASSWORD[] = {"belanegara", "raffi123"};
// nanti implementasinya hanya for (int i = 0; i < sizeof(GC_WINAME) / sizeof(GC_WINAME[0]);i++){ GC_WINAME[i]; GC_WIPASSWORD[i]; }

DHT hum(DHTPIN, DHT22);
BH1750 light;

WebServer server(80);
// // FORWARD DECLARATION // //
void handleRoot();


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

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    for (int i = 0; i < sizeof(GC_WINAME) / sizeof(GC_WINAME[0]); ++i){
      WiFi.begin(GC_WINAME[i], GC_WIPASSWORD[i]);
      if (WiFi.status() == WL_CONNECTED) break;
    }
    delay(500);
    timeout++;
  }

  // ROUTING
  server.on("/", handleRoot);
  server.begin();
}

void
loop() {
  server.handleClient();

  int micval = analogRead(MAX4466_PIN);
  float humval = hum.readHumidity();
  float tempval = hum.readTemperature();
  float luxval = light.readLightLevel();

  if (isnan(tempval) || isnan(humval)) {
    Serial.println("ERROR: DHT22 gagal baca!");
    tempval = -1;
    humval = -1;
  }

  if (WiFi.status() == WL_CONNECTED) {
    gettingToBlinky(4, 80);
  }

  Serial.println(WiFi.localIP());
  if (micval && humval && tempval && luxval){
    Serial.println("-----");
    Serial.print("Temp: ");
    Serial.print(tempval);
    Serial.println(" C");
    Serial.print("Humidity: ");
    Serial.print(humval);
    Serial.println(" %");
    Serial.print("Light: ");
    Serial.print(luxval);
    Serial.println(" lux");
    Serial.print("Sound: ");
    Serial.println(micval);
  }

  delay(1000);
}

void handleRoot() {

  // float t = dht.readTemperature();
  // float h = dht.readHumidity();
  //
  // // Validasi jika sensor gagal baca
  // if (isnan(t) || isnan(h)) {
  //   t = 0;
  //   h = 0;
  // }

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
    <h1>GELO IPSUM</h1>
    <h3>Kelompok 8</h3>


</div>

</body>
</html>
)rawliteral";

  // html.replace("%TEMP%", String(t));
  // html.replace("%HUM%", String(h));

  server.send(200, "text/html", html);

}

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
