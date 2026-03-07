#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

const char *SSID = "ESP32-Interface";
const char *PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer server(PORT);
WiFiClient client;

bool sendEnabled = false;
unsigned long sampleIntervalMs = 1000;
unsigned long lastSendMs = 0;
unsigned long streamStartMs = 0;

const uint16_t SENSOR_ID = 1;
const char *SENSOR_NAME = "mock_sensor";

void handleCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.startsWith("START")) {
    int spaceIdx = cmd.indexOf(' ');
    if (spaceIdx > 0) {
      float freqHz = cmd.substring(spaceIdx + 1).toFloat();
      if (freqHz > 0.0f) {
        sampleIntervalMs = (unsigned long)(1000.0f / freqHz);
      }
    }
    streamStartMs = millis();
    sendEnabled = true;
    Serial.printf("Streaming STARTED at %.2f Hz\n", 1000.0f / sampleIntervalMs);
    if (client && client.connected()) {
      client.print("{\"type\":\"ack\",\"cmd\":\"START\"}\n");
    }
    return;
  }

  if (cmd.startsWith("STOP")) {
    sendEnabled = false;
    Serial.println("Streaming STOPPED");
    if (client && client.connected()) {
      client.print("{\"type\":\"ack\",\"cmd\":\"STOP\"}\n");
    }
    return;
  }

  if (cmd == "PING") {
    if (client && client.connected()) {
      client.print("{\"type\":\"pong\"}\n");
    }
    return;
  }

  Serial.printf("Unknown cmd: %s\n", cmd.c_str());
  if (client && client.connected()) {
    client.print("{\"type\":\"error\",\"msg\":\"unknown_cmd\"}\n");
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASS);
  IPAddress ip = WiFi.softAPIP();
  Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());

  server.begin();
  server.setNoDelay(true);
  Serial.printf("TCP server listening on port %u\n", (unsigned int)PORT);
}

void loop() {
  WiFiClient newClient = server.available();
  if (newClient) {
    if (client && client.connected()) {
      client.stop();
    }
    client = newClient;
    client.print("{\"type\":\"status\",\"msg\":\"connected\"}\n");
    Serial.println("Client connected");
  }

  if (client && client.connected() && client.available()) {
    String cmd = client.readStringUntil('\n');
    handleCommand(cmd);
  }

  if (!(sendEnabled && client && client.connected())) {
    return;
  }

  unsigned long now = millis();
  if (now - lastSendMs < sampleIntervalMs) {
    return;
  }
  lastSendMs = now;

  float mockValue = random(0, 1000) / 10.0f;
  unsigned long timestampMs = now - streamStartMs;

  String json = "{";
  json += "\"sensor\":\"" + String(SENSOR_NAME) + "\",";
  json += "\"sensor_id\":" + String(SENSOR_ID) + ",";
  json += "\"timestamp_ms\":" + String(timestampMs) + ",";
  json += "\"value\":" + String(mockValue, 2);
  json += "}\n";

  client.print(json);
  Serial.print("Sent: ");
  Serial.print(json);
}
