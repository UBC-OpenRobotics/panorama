#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID = "ESP32_AP";
const char* AP_PASS = "12345678";

WebServer server(80);

String htmlPage() {
  return R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>ESP32 Access Point</title>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style>body{font-family:sans-serif;text-align:center;margin-top:2em;}</style>
  </head>
  <body>
    <h2>Hello from ESP32!</h2>
    <p>This page is served directly from the ESP32 Access Point.</p>
  </body>
  </html>
  )rawliteral";
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Create Access Point
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point started. Connect to Wi-Fi: ");
  Serial.println(AP_SSID);
  Serial.print("Visit: http://");
  Serial.println(IP); // usually 192.168.4.1

  // Define web routes
  server.on("/", []() { server.send(200, "text/html", htmlPage()); });
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
