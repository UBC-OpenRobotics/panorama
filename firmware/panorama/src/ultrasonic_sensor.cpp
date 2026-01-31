#include "Arduino.h"
#include "WiFi.h"

// Wi-Fi Access Point credentials
const char* SSID = "ESP32-Interface";
const char* PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer server(PORT);
WiFiClient client;

const uint16_t SENSOR_ID = 1;
const char* SENSOR_NAME = "ultrasonic";

const int trigPin = 5;
const int echoPin = 18;

long duration;
float distanceCm;

void setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASS);
    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());
}


void setupUltrasonicSensor() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

float readUltrasonicSensor() {
    // Clear the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Set the trigPin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin, returns the sound wave travel time in microseconds
    long duration = pulseIn(echoPin, HIGH, 25000);

    // Calculate the distance in centimeters
    float distanceCm = (duration * 0.0343) / 2;

    return distanceCm;
}

void handleCommand(String cmd) {
    cmd.trim();
    cmd.toUpperCase();

    if (cmd.startsWith("START")) {
        Serial.println("Ultrasonic data transmission STARTED");
    } else if (cmd.startsWith("STOP")) {
        Serial.println("Ultrasonic data transmission STOPPED");
    } else {
        Serial.printf("Unknown cmd: %s\n", cmd.c_str());
    }
}


void setup() {
    Serial.begin(115200);

    setupAccessPoint(); 
    server.begin();
    server.setNoDelay(true);

    setupUltrasonicSensor();
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

    // send ultrasonic data
    if (client && client.connected()) {
        float distance = readUltrasonicSensor();
        unsigned long timestamp = millis();

        String json =
            "{"
            "\"sensor\":\"" + String(SENSOR_NAME) + "\","
            "\"sensor_id\":" + String(SENSOR_ID) + ","
            "\"timestamp_ms\":" + String(timestamp) + ","
            "\"value\":" + String(distance, 2) +
            "}\n";

        client.print(json);
        delay(1000); // Send data every second
    }
}