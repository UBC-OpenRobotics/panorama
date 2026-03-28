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
unsigned long startTime = 0;
unsigned long lastBlink = 0;
bool ledState = false;

uint32_t seq = 0;

struct SensorConfig {
  const char* sensor;
  const char* dataunit;
  const char* datatype;
  uint16_t sensorID;
};

const SensorConfig ultrasonicSensor = {
  "US1000",
  "cm",
  "distance",
  4,
};

float readUltrasonicCm() {
  // ensure trigger is low
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 10 µs HIGH pulse to trigger measurement
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure echo time (timeout 30 ms ≈ 5 m)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    return -1.0f; // no echo
  }

  // speed of sound ≈ 0.0343 cm/µs, divide by 2 (out and back)
  float distanceCm = (duration * 0.0343f) / 2.0f;
  return distanceCm;
}

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
    startTime = millis();
    seq = 0;              // reset sequence on start
    sendEnabled = true;
    Serial.println("Signal ON");

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

    const SensorConfig& s = ultrasonicSensor;
    float value = readUltrasonicCm();
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
