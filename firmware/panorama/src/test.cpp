#include <WiFi.h>
#include <Arduino.h>

// Wi-Fi Access Point credentials
const char* SSID = "ESP32-Interface";
const char* PASS = "12345678";
const uint16_t PORT = 9000;

// HC-SR04 pins (adjust to match your wiring)
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;

WiFiServer server(PORT);
WiFiClient client;

bool sendEnabled = false;
unsigned long sampleInterval = 1000; // ms
unsigned long lastSend = 0;
unsigned long startTime = 0;

uint32_t seq = 0;
const uint16_t SENSOR_ID = 1;
const char* SENSOR_NAME = "ultrasonic_distance_cm";

// Return distance in cm, or -1.0 if no echo / timeout
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

  // read commands
  if (client && client.connected() && client.available()) {
    String cmd = client.readStringUntil('\n');
    handleCommand(cmd);
  }

  // send JSON data packets
  if (sendEnabled && client && client.connected()) {
    unsigned long now = millis();
    if (now - lastSend >= sampleInterval) {
      lastSend = now;

      float distanceCm = readUltrasonicCm();
      unsigned long timestamp = now - startTime;

      String json =
        "{"
          "\"sensor\":\"" + String(SENSOR_NAME) + "\","
          "\"sensor_id\":" + String(SENSOR_ID) + ","
          "\"seq\":" + String(seq++) + ","
          "\"timestamp_ms\":" + String(timestamp) + ","
          "\"value\":" + String(distanceCm, 2) +
        "}\n";

      client.print(json);
      Serial.print("Sent: ");
      Serial.print(json);
    }
  }
}