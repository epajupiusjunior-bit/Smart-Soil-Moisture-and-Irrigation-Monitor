// Smart Soil Moisture & Irrigation Monitor - Wokwi Simulation
// For Uganda IoT Project

#include <DHT.h>

// Pin Definitions
#define SOIL_PIN     34   // Potentiometer (Soil Moisture)
#define DHT_PIN      15   // DHT22
#define PUMP_PIN     22   // LED simulating Water Pump
#define DHT_TYPE     DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);   // Pump starts OFF
  
  dht.begin();
  
  Serial.println("Smart Irrigation System Started");
  delay(2000);
}

void loop() {
  // Read Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  int moisturePercent = map(soilRaw, 0, 4095, 100, 0);  // Convert to percentage
  
  // Read DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Print readings
  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("% | Temp: ");
  Serial.print(temperature);
  Serial.print("°C | Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  // Control Logic
  if (moisturePercent < 35) {           // Change threshold as needed
    digitalWrite(PUMP_PIN, HIGH);       // Turn ON Pump (LED)
    Serial.println("🚰 PUMP ON - Irrigating...");
  } 
  else {
    digitalWrite(PUMP_PIN, LOW);        // Turn OFF Pump
    Serial.println("✅ Soil is moist - Pump OFF");
  }

  // Small delay
  delay(3000);   // Read every 3 seconds (change later for real use)
}