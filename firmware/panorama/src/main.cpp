// #include <Arduino.h>
// #include <WiFi.h>

// // ===== Wi-Fi Access Point (ESP32 hosts itself) =====
// const char* AP_SSID = "ESP32-JSON";
// const char* AP_PASS = "esp32json";    // must be 8+ chars

// // ===== TCP server =====
// const uint16_t SERVER_PORT = 9000;
// WiFiServer server(SERVER_PORT);

// // ===== State =====
// WiFiClient client;            // single client for simplicity
// unsigned long lastSendMs = 0; // pacing JSON sends
// const uint32_t SEND_PERIOD_MS = 1000;

// // make a tiny JSON line without ArduinoJson
// String makeJson() {
//   // dummy data — tweak as you like
//   static int seq = 0;
//   float temp = 20.0 + sin(millis() / 1000.0) * 2.5;
//   int humidity = 40 + (millis()/1000) % 20;

//   // ISO-ish time in ms since boot for now
//   String s = "{";
//   s += "\"seq\":" + String(seq++) + ",";
//   s += "\"ts_ms\":" + String(millis()) + ",";
//   s += "\"temp_c\":" + String(temp, 2) + ",";
//   s += "\"humidity\":" + String(humidity) + ",";
//   s += "\"status\":\"ok\"";
//   s += "}\n";                   // newline-delimited JSON (NDJSON)
//   return s;
// }

// void setup() {
//   Serial.begin(115200);
//   delay(200);

//   // Start AP (self-hosted)
//   Serial.println("[WiFi] Starting AP…");
//   bool ok = WiFi.softAP(AP_SSID, AP_PASS);
//   if (!ok) {
//     Serial.println("[WiFi] AP start failed!");
//   }
//   IPAddress ip = WiFi.softAPIP(); // usually 192.168.4.1
//   Serial.print("[WiFi] AP SSID: "); Serial.println(AP_SSID);
//   Serial.print("[WiFi] AP PASS: "); Serial.println(AP_PASS);
//   Serial.print("[WiFi] AP IP:   "); Serial.println(ip);

//   // Start TCP server
//   server.begin();
//   server.setNoDelay(true);
//   Serial.print("[TCP] Listening on port "); Serial.println(SERVER_PORT);
// }

// void handleNewClient() {
//   WiFiClient incoming = server.available();
//   if (!incoming) return;

//   // If we already have a client, drop the older one
//   if (client && client.connected()) {
//     client.stop();
//   }

//   client = incoming;
//   client.setTimeout(50);
//   Serial.print("[TCP] Client connected from ");
//   Serial.println(client.remoteIP());

//   // optional greeting / one-shot JSON blob
//   client.print("{\"hello\":\"welcome\",\"port\":");
//   client.print(SERVER_PORT);
//   client.print(",\"hint\":\"I will stream one JSON per second. Each line is a JSON object.\"}\n");
// }

// void streamJsonIfTime() {
//   if (!client || !client.connected()) return;

//   // read & ignore any input (you could add commands here)
//   while (client.available()) {
//     (void)client.read(); // drain input
//   }

//   unsigned long now = millis();
//   if (now - lastSendMs >= SEND_PERIOD_MS) {
//     lastSendMs = now;
//     String line = makeJson();
//     client.print(line);     // send one JSON line
//     Serial.print("[TX] ");  // mirror to serial
//     Serial.print(line);
//   }
// }

// void loop() {
//   // accept new client connections
//   handleNewClient();

//   // send JSON periodically if a client is connected
//   streamJsonIfTime();

//   // clean up if disconnected
//   if (client && !client.connected()) {
//     Serial.println("[TCP] Client disconnected");
//     client.stop();
//   }
// }

