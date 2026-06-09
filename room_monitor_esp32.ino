// =========================================
//   ESP32 Room Monitor
//   Sensor: DHT22, KY-038, MQ135, HC-SR501, BH1750
// =========================================

#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

// --- WiFi Credentials ---
const char* ssid     = "HOTSPOT@UPNJATIM.AC.ID";
const char* password = "belanegara";

// --- Pin Definitions ---
#define DHT_PIN       4
#define DHT_TYPE      DHT22
#define SOUND_AO_PIN  34   // KY-038 Analog
#define SOUND_DO_PIN  35   // KY-038 Digital
#define MQ135_PIN     32   // MQ135 Analog
#define PIR_PIN       13   // HC-SR501

// --- I2C Pins (BH1750) ---
#define I2C_SDA 21
#define I2C_SCL 22

// --- Objects ---
DHT dht(DHT_PIN, DHT_TYPE);
BH1750 lightMeter;

// --- Variables ---
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 2000; // baca sensor tiap 2 detik

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== ESP32 Room Monitor ===");

  // Init pins
  pinMode(PIR_PIN, INPUT);
  pinMode(SOUND_DO_PIN, INPUT);

  // Konfigurasi ADC ESP32
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Init DHT22
  dht.begin();

  // Init BH1750 via I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750 OK");
  } else {
    Serial.println("BH1750 TIDAK DITEMUKAN - cek kabel SDA/SCL");
  }

  // Koneksi WiFi
  Serial.print("Menghubungkan ke WiFi");
  // WiFi.begin(ssid, password);
  // int tries = 0;
  // while (WiFi.status() != WL_CONNECTED && tries < 20) {
  //   delay(500);
  //   Serial.print(".");
  //   tries++;
  // }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Terhubung!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nGagal konek WiFi, lanjut tanpa WiFi.");
  }

  Serial.println("\n--- Mulai Monitoring ---");
  Serial.println("Suhu(C) | Kelembaban(%) | Cahaya(lux) | Suara | Gas(MQ135) | Gerak");
}

void loop() {
  unsigned long now = millis();

  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;

    // --- DHT22: Suhu & Kelembaban ---
    float temperature = dht.readTemperature();
    float humidity    = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("ERROR: DHT22 gagal baca!");
      temperature = -1;
      humidity = -1;
    }

    // --- BH1750: Cahaya ---
    float lux = lightMeter.readLightLevel();

    // --- KY-038: Suara ---
    // Ambil 20 sampel untuk mengurangi noise
    long total = 0;

    for(int i = 0; i < 20; i++) {
      total += analogRead(SOUND_AO_PIN);
      delay(2);
    }

    int soundAnalog = total / 20;
    int soundDigital = digitalRead(SOUND_DO_PIN);

    // --- MQ135: Gas / Kualitas Udara ---
    // Nilai raw: 0-4095. Semakin tinggi = udara lebih buruk
    int gasRaw = analogRead(MQ135_PIN);
    // Konversi kasar ke ppm (perlu kalibrasi untuk hasil akurat)
    float gasPPM = map(gasRaw, 0, 4095, 0, 1000);

    // --- HC-SR501: Gerakan ---
    int motion = digitalRead(PIR_PIN); // HIGH = ada gerakan

    // --- Tampilkan ke Serial Monitor ---
    Serial.print(temperature, 1);
    Serial.print("°C | ");
    Serial.print(humidity, 1);
    Serial.print("% | ");
    Serial.print(lux, 1);
    Serial.print(" lux | ");
    Serial.print(soundAnalog);
    Serial.print(" level | Gas:");
    Serial.print(gasRaw);
    Serial.print(" (~");
    Serial.print(gasPPM, 0);
    Serial.print("ppm) | Gerak:");
    Serial.println(motion ? "TERDETEKSI" : "-");

    // --- Alert sederhana ---
    checkAlerts(temperature, humidity, gasPPM, motion, soundDigital);
  }
}

void checkAlerts(float temp, float hum, float gas, int motion, int loudSound) {
  if (temp > 35.0) {
    Serial.println("  ⚠ PERINGATAN: Suhu terlalu tinggi!");
  }
  if (hum > 80.0) {
    Serial.println("  ⚠ PERINGATAN: Kelembaban terlalu tinggi!");
  }
  if (gas > 700) {
    Serial.println("  ⚠ PERINGATAN: Kualitas udara buruk!");
  }
  if (motion) {
    Serial.println("  → Gerakan terdeteksi di ruangan");
  }
  if (loudSound) {
    Serial.println("  → Suara keras terdeteksi");
  }
}
