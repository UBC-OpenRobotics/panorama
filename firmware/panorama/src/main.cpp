#include "i2c_protocol.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <Wire.h>

// Wi-Fi Access Point credentials
const char *SSID = "ESP32-Interface";
const char *PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer server(PORT);
WiFiClient client;

bool sendEnabled = false;
unsigned long sampleIntervalMs = 1000;
unsigned long lastSendMs = 0;

const int I2C_SDA_PIN = 21;
const int I2C_SCL_PIN = 22;
const uint32_t I2C_CLOCK_HZ = 100000;

const uint16_t SENSOR_ID = 1;
const char *SENSOR_NAME = "temperature_i2c";

uint8_t sensorAddresses[MAX_I2C_SENSORS];
size_t sensorCount = 0;

void setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASS);
    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());
}

bool readTemperatureC(uint8_t address, float &temperatureC) {
    // Common I2C temperature sensors expose temperature at register 0x00.
    Wire.beginTransmission(address);
    Wire.write((uint8_t)0x00);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    uint8_t data[2] = {0, 0};
    if (!readFromI2C(address, data, sizeof(data))) {
        return false;
    }

    // TMP102-style 12-bit signed temperature conversion.
    int16_t raw = ((int16_t)data[0] << 8) | data[1];
    raw >>= 4;
    if (raw & 0x0800) {
        raw |= 0xF000;
    }

    temperatureC = raw * 0.0625f;
    return true;
}

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
        sendEnabled = true;
        Serial.printf("I2C temperature streaming STARTED at %.2f Hz\n", 1000.0f / sampleIntervalMs);
        return;
    }

    if (cmd.startsWith("STOP")) {
        sendEnabled = false;
        Serial.println("I2C temperature streaming STOPPED");
        return;
    }

    if (cmd == "RESCAN") {
        sensorCount = scanForI2CDevices(sensorAddresses, MAX_I2C_SENSORS);
        return;
    }

    Serial.printf("Unknown cmd: %s\n", cmd.c_str());
}

void setup() {
    Serial.begin(115200);
    setupAccessPoint();
    server.begin();
    server.setNoDelay(true);

    setupI2C(I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_HZ);
    sensorCount = scanForI2CDevices(sensorAddresses, MAX_I2C_SENSORS);
}

void loop() {
    WiFiClient newClient = server.available();
    if (newClient) {
        if (client && client.connected()) {
            client.stop();
        }
        client = newClient;
        client.print("{\"type\":\"status\",\"msg\":\"connected\"}\n");
        Serial.println("Backend connected");
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

    if (sensorCount == 0) {
        client.print("{\"type\":\"warning\",\"msg\":\"no_i2c_sensors\"}\n");
        return;
    }

    for (size_t i = 0; i < sensorCount; i++) {
        uint8_t address = sensorAddresses[i];
        float temperatureC = 0.0f;
        bool success = readTemperatureC(address, temperatureC);

        String json = "{";
        json += "\"sensor\":\"" + String(SENSOR_NAME) + "\",";
        json += "\"sensor_id\":" + String(SENSOR_ID) + ",";
        json += "\"i2c_addr\":\"0x" + String(address, HEX) + "\",";
        json += "\"timestamp_ms\":" + String(now) + ",";
        json += "\"success\":" + String(success ? "true" : "false");
        if (success) {
            json += ",\"value_c\":" + String(temperatureC, 2);
        }
        json += "}\n";

        client.print(json);
    }
}
