// Smart Soil Moisture & Irrigation Monitor - Uganda Project
#include <DHT.h>

// Pin Definitions
#define SOIL_PIN     34
#define DHT_PIN      15
#define GREEN_LED    17   // Irrigating / Pump ON
#define RED_LED      22   // Soil is Moist (Good)
#define DHT_TYPE     DHT22

DHT dht(DHT_PIN, DHT_TYPE);

bool errorState = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  // Start both OFF
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  
  dht.begin();
  
  Serial.println("=== Smart Irrigation System Started ===");
  Serial.println("Turn potentiometer to change soil moisture");
  delay(2000);
}

void blinkError() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    delay(300);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    delay(300);
  }
}

void loop() {
  // Read Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  int moisturePercent = map(soilRaw, 0, 4095, 100, 0);

  // Read DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check for sensor error
  if (isnan(humidity) || isnan(temperature)) {
    if (!errorState) {
      Serial.println("❌ DHT Sensor Error!");
      errorState = true;
    }
    blinkError();
    return;                    // Skip normal operation
  }

  errorState = false;

  // Print readings
  Serial.printf("Soil Moisture: %d%% | Temp: %.1f°C | Humidity: %.1f%%\n", 
                moisturePercent, temperature, humidity);

  // Control Logic + LEDs
  if (moisturePercent < 35) {
    // DRY → Irrigate
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    Serial.println("🚰 IRRIGATING... (Green LED ON)");
  } 
  else {
    // MOIST → Good condition
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    Serial.println("✅ Soil is Moist (Red LED ON)");
  }

  delay(2500);   // Update every 2.5 seconds
}