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

/*
const char* GC_WINAME[] = {"HOTSPOT@UPNJATIM.AC.ID"}; // tambahkan wifi terus, beserta dengan pwnya
const char* GC_WIPASSWORD[] = {"belanegara"};
*/
const char* GC_WINAME[] = {"Alvhim"}; // tambahkan wifi terus, beserta dengan pwnya
const char* GC_WIPASSWORD[] = {"tahukotak"};

// IPAddress staticIP(172, 27, 205, 150);
// IPAddress gateway(172, 27, 205, 1);
// IPAddress subnet(255, 255, 255, 0);

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


  // WiFi.mode(WIFI_STA);
  // WiFi.config(staticIP, gateway, subnet);

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
  server.enableCORS(true);
  server.on("/", handleScream);
  server.begin();
}

void
loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.localIP());
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
