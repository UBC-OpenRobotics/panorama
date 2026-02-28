#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

// Wi-Fi Access Point credentials
const char* SSID = "ESP32-Interface";
const char* PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer server(PORT);
WiFiClient client;

const int trigPin = 5;
const int echoPin = 18;
const unsigned long send_rate = 1000; // milliseconds

long duration;
float distanceCm;

const int pinsToScan[] = {5, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
const int numPins = sizeof(pinsToScan) / sizeof(pinsToScan[0]);
bool sensors[numPins] = {false};

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

    for (int i = 0; i < numPins; i++) {
        pinMode(pinsToScan[i], INPUT_PULLUP);
    }

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
    static unsigned long lastSend = 0;

    if (client && client.connected()) {
        client.print("Polling for connected sensors");
        for (int i = 0; i < numPins; i++) {
            if (digitalRead(pinsToScan[i]) == LOW) {
                client.print("Sensor detected at GPIO ");
                client.print(pinsToScan[i]);
                client.print("\n");
                sensors[i] = true;
            }
        }

        

        // for (int i = 0; i < numPins; i++) {
        //     if (sensors[i]) {
        //         trigPin = pinsToScan[i];
        //         sensors[i] = false;
        //         break;
        //     }
        // }
        // if (millis() - lastSend >= send_rate) {
        // lastSend = millis();
        // float distance = readUltrasonicSensor();
        // unsigned long timestamp = millis();

        // String json =
        //     "{"
        //     "\"sensor\":\"" + String(SENSOR_NAME) + "\","
        //     "\"sensor_id\":" + String(SENSOR_ID) + ","
        //     "\"timestamp_ms\":" + String(timestamp) + ","
        //     "\"value\":" + String(distance, 2) +
        //     "}\n";

        // client.print(json);
        // }
    }
    return;
}


/*
Notes:
how can we figure out which sensor is connected to which pin
    - should we scan all pins every loop, or just one pin at a time
    - maybe when backend connects can send message indicating type of sensor
      and which pins to scan
    - use i2c to identify addresses of connected sensors
for how we are sending data
    - right now sending JSON strings over TCP
    - do we want to switch to use HTTP packets instead of strings?
*/