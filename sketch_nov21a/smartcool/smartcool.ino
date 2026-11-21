
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "SmartCoolModel.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Wi-Fi settings
// Enter your existing Wi-Fi credentials here.
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* SERVER_URL =
  "http://192.168.43.22:5000/api/readings";

// ESP32 pin assignments
#define LDR_PIN 34
#define MQ7_PIN 35
#define FAN_PIN 27
#define LED_PIN 26

// Temperature thresholds
const float FAN_ON_TEMP = 30.0;
const float FAN_OFF_TEMP = 28.0;

// Gas warning threshold
const int GAS_WARNING_LEVEL = 2500;

// System states
bool fanStatus = false;
bool gasWarning = false;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");

  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(FAN_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  analogReadResolution(12);

  if (!sht31.begin(0x45)) {
    Serial.println("ERROR: SHT31 NOT DETECTED");
  } else {
    Serial.println("SHT31 INITIALIZED");
  }

  connectWiFi();

  Serial.println();
  Serial.println("SMARTCOOL SYSTEM STARTED");
  Serial.println("EDGE MACHINE LEARNING ENABLED");
}

void loop() {

  // 1. Read temperature and humidity
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  // 2. Read light and gas sensors
  int lightValue = analogRead(LDR_PIN);
  int gasValue = analogRead(MQ7_PIN);

  // 3. Temperature-based fan control
  // Existing control logic is preserved.

  if (!isnan(temperature)) {

    if (temperature >= FAN_ON_TEMP) {
      fanStatus = true;
    }
    else if (temperature <= FAN_OFF_TEMP) {
      fanStatus = false;
    }
  }

  digitalWrite(FAN_PIN, fanStatus ? HIGH : LOW);

  // 4. Gas warning LED control
  gasWarning = (gasValue >= GAS_WARNING_LEVEL);

  digitalWrite(LED_PIN, gasWarning ? HIGH : LOW);

  // 5. EDGE MACHINE LEARNING PREDICTION

  int overheatingPrediction = -1;
  unsigned long inferenceTime = 0;

  if (!isnan(temperature) && !isnan(humidity)) {

    // The model expects six features:
    // Front temperature, Front humidity,
    // Middle temperature, Middle humidity,
    // Back temperature, Back humidity.
    //
    // Current prototype has one SHT31.
    // Its readings are repeated as a temporary
    // approximation for the three greenhouse zones.

    float modelInputs[6] = {
      temperature, humidity,
      temperature, humidity,
      temperature, humidity
    };

    // Measure local inference time
    unsigned long startTime = micros();

    overheatingPrediction = predictSmartCool(modelInputs);

    inferenceTime = micros() - startTime;
  }

  // 6. Display readings and ML results
  Serial.println();
  Serial.println("========= SMARTCOOL =========");

  if (!isnan(temperature)) {
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.println(" C");
  } else {
    Serial.println("Temperature: SENSOR ERROR");
  }

  if (!isnan(humidity)) {
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %");
  } else {
    Serial.println("Humidity: SENSOR ERROR");
  }

  Serial.print("Gas level (raw): ");
  Serial.println(gasValue);

  Serial.print("Light intensity (raw): ");
  Serial.println(lightValue);

  Serial.print("Fan status: ");
  Serial.println(fanStatus ? "ON" : "OFF");

  Serial.print("Gas warning LED: ");
  Serial.println(gasWarning ? "ON - WARNING" : "OFF - NORMAL");

  Serial.println("-----------------------------");
  Serial.println("EDGE ML RESULTS");

  if (overheatingPrediction != -1) {

    Serial.print("ML prediction: ");

    if (overheatingPrediction == 1) {
      Serial.println("OVERHEATING");
    } else {
      Serial.println("NORMAL");
    }

    Serial.print("Inference time: ");
    Serial.print(inferenceTime);
    Serial.println(" microseconds");

  } else {
    Serial.println("ML prediction: UNAVAILABLE");
  }

  // 7. Send readings and local ML prediction to Flask

  if (WiFi.status() == WL_CONNECTED &&
      !isnan(temperature) &&
      !isnan(humidity)) {

    HTTPClient http;

    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);

    String jsonData = "{";

    jsonData += "\"temperature\":" + String(temperature, 2) + ",";
    jsonData += "\"humidity\":" + String(humidity, 2) + ",";
    jsonData += "\"light\":" + String(lightValue) + ",";
    jsonData += "\"gas\":" + String(gasValue) + ",";
    jsonData += "\"fan\":";
    jsonData += (fanStatus ? "true" : "false");
    jsonData += ",";

    jsonData += "\"gas_warning\":";
    jsonData += (gasWarning ? "true" : "false");
    jsonData += ",";

    jsonData += "\"ml_prediction\":";
    jsonData += String(overheatingPrediction);
    jsonData += ",";

    jsonData += "\"ml_status\":\"";

    if (overheatingPrediction == 1) {
      jsonData += "OVERHEATING";
    }
    else if (overheatingPrediction == 0) {
      jsonData += "NORMAL";
    }
    else {
      jsonData += "UNAVAILABLE";
    }

    jsonData += "\",";
    jsonData += "\"inference_time_us\":";
    jsonData += String(inferenceTime);

    jsonData += "}";

    Serial.println("-----------------------------");
    Serial.println("Sending data to Flask...");

    int httpResponseCode = http.POST(jsonData);

    Serial.print("HTTP response: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {

      Serial.println("Data sent successfully!");

      String response = http.getString();
      Serial.println("Server response:");
      Serial.println(response);

    } else {

      Serial.println("Failed to send readings.");

      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();

  } else {

    Serial.println("Wi-Fi disconnected or sensor error.");
  }

  Serial.println("=============================");

  delay(2000);
}
