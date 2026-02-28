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

// Stream control
bool sendEnabled = false;
unsigned long sampleIntervalMs = 1000;
unsigned long lastSendMs = 0;

// I2C bus configuration
const int I2C_SDA_PIN = 21;
const int I2C_SCL_PIN = 22;
const uint32_t I2C_CLOCK_HZ = 100000;

const uint8_t SENSOR_READ_LEN = 2;
const uint16_t SENSOR_ID = 2;
const char *SENSOR_NAME = "ultrasonic_i2c";

uint8_t sensorAddresses[16];
size_t sensorCount = 0;

void setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASS);
    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());
}

void setupI2C() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_HZ);
    Serial.printf("I2C started on SDA=%d SCL=%d @ %lu Hz\n", I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_HZ);
}

void scanForI2CDevices() {
    sensorCount = 0;

    for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            if (sensorCount < (sizeof(sensorAddresses) / sizeof(sensorAddresses[0]))) {
                sensorAddresses[sensorCount++] = address;
            }
            Serial.printf("I2C device found at 0x%02X\n", address);
        }
    }

    if (sensorCount == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.printf("Total I2C devices tracked: %u\n", (unsigned int)sensorCount);
    }
}

bool readDistanceCmFromI2C(uint8_t address, float &distanceCm) {
    int received = Wire.requestFrom((int)address, (int)SENSOR_READ_LEN);
    if (received != SENSOR_READ_LEN) {
        while (Wire.available()) {
            (void)Wire.read();
        }
        return false;
    }

    uint16_t raw = ((uint16_t)Wire.read() << 8) | Wire.read();
    distanceCm = raw / 10.0f;
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
        Serial.printf("I2C streaming STARTED at %.2f Hz\n", 1000.0f / sampleIntervalMs);
        return;
    }

    if (cmd.startsWith("STOP")) {
        sendEnabled = false;
        Serial.println("I2C streaming STOPPED");
        return;
    }

    if (cmd == "RESCAN") {
        scanForI2CDevices();
        return;
    }

    Serial.printf("Unknown cmd: %s\n", cmd.c_str());
}

void setup() {
    Serial.begin(115200);
    setupAccessPoint();
    server.begin();
    server.setNoDelay(true);

    setupI2C();
    scanForI2CDevices();
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
        float distanceCm = 0.0f;
        bool ok = readDistanceCmFromI2C(address, distanceCm);

        String json = "{";
        json += "\"sensor\":\"" + String(SENSOR_NAME) + "\",";
        json += "\"sensor_id\":" + String(SENSOR_ID) + ",";
        json += "\"i2c_addr\":\"0x" + String(address, HEX) + "\",";
        json += "\"timestamp_ms\":" + String(now) + ",";
        json += "\"ok\":" + String(ok ? "true" : "false");
        if (ok) {
            json += ",\"value_cm\":" + String(distanceCm, 2);
        }
        json += "}\n";

        client.print(json);
    }
}
