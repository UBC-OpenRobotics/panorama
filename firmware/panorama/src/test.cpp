#include <WiFi.h>
#include <Arduino.h>

// Wi-Fi Access Point credentials
const char* SSID = "ESP32-Interface";
const char* PASS = "12345678";
const uint16_t PORT = 9000;

// Onboard LED pin
const int LED_PIN = 2;

// HC-SR04 pins (adjust to match your wiring)
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const unsigned long BLINK_DELAY_MS = 250;

WiFiServer server(PORT);
WiFiClient client;

bool sendEnabled = false;
unsigned long sampleInterval = 1000; // ms
unsigned long lastSend = 0;
unsigned long startTime = 0;
unsigned long lastBlink = 0;
bool ledState = false;

uint32_t seq = 0;

struct SensorConfig {
  const char* sensor;
  const char* dataunit;
  const char* datatype;
  uint16_t sensorID;
  float baseValue;
  float variation;
};

const SensorConfig sensors[] = {
  {"TM1000", "K",            "temperature", 1, 25.5f,    0.5f},
  {"HD1000", "watercontent",  "humidity",    2, 60.2f,    0.3f},
  {"PP1000", "hPa",          "pressure",    3, 1013.25f, 0.1f},
  {"NM1000", "nm",           "light",       4, 0.2f,     0.0f},
};
const int NUM_SENSORS = sizeof(sensors) / sizeof(sensors[0]);

void setup() {
  Serial.begin(115200);

  // onboard LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());

  server.begin();
  server.setNoDelay(true);
}

void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.startsWith("START")) {
    int spaceIdx = cmd.indexOf(' ');
    if (spaceIdx > 0) {
      float freq = cmd.substring(spaceIdx + 1).toFloat();
      if (freq > 0) sampleInterval = 1000.0 / freq;
    }
    startTime = millis();
    seq = 0;              // reset sequence on start
    sendEnabled = true;
    Serial.printf("Signal ON, freq=%.1f Hz\n", 1000.0 / sampleInterval);

  } else if (cmd.startsWith("STOP")) {
    sendEnabled = false;
    Serial.println("Signal OFF");

  } else {
    Serial.printf("Unknown cmd: %s\n", cmd.c_str());
  }
}

void loop() {
  // accept new client
  WiFiClient newClient = server.available();
  if (newClient) {
    if (client && client.connected()) client.stop();
    client = newClient;
    client.print(F("{\"type\":\"status\",\"msg\":\"connected\"}\n"));
    Serial.println("Backend connected");
  }

  // update onboard LED: solid when connected, blink when disconnected
  if (client && client.connected()) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    unsigned long now = millis();
    if (now - lastBlink >= BLINK_DELAY_MS) {
      lastBlink = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  }

  // read commands
  if (client && client.connected() && client.available()) {
    String cmd = client.readStringUntil('\n');
    handleCommand(cmd);
  }

  if (sendEnabled && client && client.connected()) {
    unsigned long now = millis();
    if (now - lastSend >= sampleInterval) {
      lastSend = now;

      int idx = seq % NUM_SENSORS;
      const SensorConfig& s = sensors[idx];
      float value = s.baseValue + (seq % 10) * s.variation;
      unsigned long timestamp = now - startTime;

      String json =
        "{"
          "\"sensor\":\"" + String(s.sensor) + "\","
          "\"dataunit\":\"" + String(s.dataunit) + "\","
          "\"data\":" + String(value, 2) + ","
          "\"datatype\":\"" + String(s.datatype) + "\","
          "\"sensorID\":" + String(s.sensorID) + ","
          "\"seq\":" + String(seq++) + ","
          "\"timestamp\":" + String(timestamp) +
        "}\n";

      client.print(json);
      Serial.print("Sent: ");
      Serial.print(json);
    }
  }
}