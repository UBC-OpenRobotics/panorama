#include "i2c_protocol.h"
#include "sensor_data.h"
#include "temperature_sensor.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

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
const uint8_t LM335_ADC_PIN = 34;

const uint16_t SENSOR_ID = 1;
const char *SENSOR_NAME = "temperature_lm335_adc";

uint8_t sensorAddresses[MAX_I2C_SENSORS];
size_t sensorCount = 0;

void setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASS);
    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());
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
        Serial.printf("Temperature streaming STARTED at %.2f Hz\n", 1000.0f / sampleIntervalMs);
        return;
    }

    if (cmd.startsWith("STOP")) {
        sendEnabled = false;
        Serial.println("Temperature streaming STOPPED");
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
    setupTemperatureSensor(LM335_ADC_PIN);
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

    float temperatureC = readTemperature(true);

    SensorData data = {
        SENSOR_NAME,
        SENSOR_ID,
        now,
        'C', // maybe use condtional on global bool for celsius vs fahrenheit
        temperatureC};

    client.print(sensorDataToJson(data));
}
