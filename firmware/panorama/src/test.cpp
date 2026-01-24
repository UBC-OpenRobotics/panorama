#include <WiFi.h>

// Wi-Fi Access Point credentials
const char* SSID = "ESP32-Interface";
const char* PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer server(PORT);
WiFiClient client;

bool sendEnabled = false;
unsigned long sampleInterval = 1000; // ms
unsigned long lastSend = 0;
unsigned long startTime = 0;

uint32_t seq = 0;
const uint16_t SENSOR_ID = 1;
const char* SENSOR_NAME = "temperature";

void setup() {
  Serial.begin(115200);

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
    client.print("{\"type\":\"status\",\"msg\":\"connected\"}\n");
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

      float sensorVal = random(0, 1000) / 10.0;
      unsigned long timestamp = now - startTime;

      String json =
        "{"
          "\"sensor\":\"" + String(SENSOR_NAME) + "\","
          "\"sensor_id\":" + String(SENSOR_ID) + ","
          "\"seq\":" + String(seq++) + ","
          "\"timestamp_ms\":" + String(timestamp) + ","
          "\"value\":" + String(sensorVal, 2) +
        "}\n";

      client.print(json);
      Serial.print("Sent: ");
      Serial.print(json);
    }
  }
}
