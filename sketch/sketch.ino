// Smart Soil Moisture & Irrigation System - Uganda IoT Project
// With WiFi + ThingSpeak Dashboard

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// ================== CONFIGURATION ==================
const char* ssid = "Wokwi-GUEST";           // Your WiFi name
const char* password = "";   // Your WiFi password

const char* serverName = "http://api.thingspeak.com/update";
String apiKey = "YOUR_API_KEY";           // ThingSpeak Write API Key

// Pin Definitions
#define SOIL_PIN     34
#define DHT_PIN      15
#define GREEN_LED    17     // Irrigating (Pump ON)
#define RED_LED      22     // Soil is Moist (Good)
#define DHT_TYPE     DHT22

DHT dht(DHT_PIN, DHT_TYPE);

int moisturePercent = 0;
float temperature = 0;
float humidity = 0;
bool pumpStatus = false;
bool errorState = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  dht.begin();

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n✅ WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("=== Smart Irrigation System Ready ===");
  delay(2000);
}

void blinkError() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    delay(250);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    delay(250);
  }
}

void sendToThingSpeak() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    
    String postData = "api_key=" + apiKey +
                      "&field1=" + String(moisturePercent) +
                      "&field2=" + String(temperature) +
                      "&field3=" + String(humidity) +
                      "&field4=" + String(pumpStatus);

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    
    if (httpResponseCode > 0) {
      Serial.print("Data sent to ThingSpeak. Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("❌ Error sending data: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

void loop() {
  // Read Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  moisturePercent = map(soilRaw, 0, 4095, 100, 0);

  // Read DHT
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Error Handling
  if (isnan(humidity) || isnan(temperature)) {
    if (!errorState) {
      Serial.println("❌ DHT Sensor Error!");
      errorState = true;
    }
    blinkError();
    return;
  }
  
  errorState = false;

  // Control Logic
  if (moisturePercent < 35) {
    // Dry Soil → Irrigate
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    pumpStatus = true;
    Serial.println("🚰 IRRIGATING... (Green LED ON)");
  } else {
    // Moist Soil → Good
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    pumpStatus = false;
    Serial.println("✅ Soil is Moist (Red LED ON)");
  }

  // Print readings
  Serial.printf("Moisture: %d%% | Temp: %.1f°C | Humidity: %.1f%% | Pump: %s\n",
                moisturePercent, temperature, humidity, pumpStatus ? "ON" : "OFF");

  // Send data to ThingSpeak every 15 seconds (ThingSpeak minimum)
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 15000) {
    sendToThingSpeak();
    lastSend = millis();
  }

  delay(2000);
}