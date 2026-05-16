//Individual ENd of Semester Project
#define BLYNK_TEMPLATE_ID "TMPL2PdbpyeP5"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation Uganda"
#define BLYNK_AUTH_TOKEN "1HtPGlLkvqeZmvhQ0JjHmP5-_iuCQ4SN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Pins
#define SOIL_PIN     34
#define DHT_PIN      15
#define GREEN_LED    17     // Irrigating (Pump ON)
#define RED_LED      22     // Soil Moist

#define DHT_TYPE     DHT22

DHT dht(DHT_PIN, DHT_TYPE);

// WiFi for Wokwi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

int moisturePercent = 0;
float temperature = 0;
float humidity = 0;
bool pumpStatus = false;

BlynkTimer timer;

void sendSensorData() {
  int soilRaw = analogRead(SOIL_PIN);
  moisturePercent = map(soilRaw, 0, 4095, 100, 0);

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT Error!");
    return;
  }

  // Control Logic
  if (moisturePercent < 35) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    pumpStatus = true;
    Serial.println("🚰 Pump ON");
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    pumpStatus = false;
    Serial.println("✅ Soil Moist");
  }

  // Send data to Blynk App
  Blynk.virtualWrite(V0, moisturePercent);     // Soil Moisture
  Blynk.virtualWrite(V1, temperature);         // Temperature
  Blynk.virtualWrite(V2, humidity);            // Humidity
  Blynk.virtualWrite(V3, pumpStatus ? 1 : 0);  // Pump Status
}

BLYNK_WRITE(V4) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    // Manual override: Force Pump ON
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    Serial.println("Manual Pump ON from App");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  dht.begin();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, sendSensorData);   // Send data every 3 seconds
  Serial.println("System Started!");
}

void loop() {
  Blynk.run();
  timer.run();
}