#include <WiFi.h>
#include <Arduino.h>
#include "panorama_server.h"

// Onboard LED pin
const int LED_PIN = 2;

// HC-SR04 pins (adjust to match your wiring)
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const unsigned long BLINK_DELAY_MS = 250;

PanoramaServer panoramaServer;

bool sendEnabled = false;
unsigned long sampleInterval = 1000; // ms
unsigned long lastSend = 0;
unsigned long startTime = 0;
unsigned long lastBlink = 0;
bool ledState = false;

uint32_t seq = 0;

struct SensorConfig {
  const char* sensor;
  const char* dataunit;
  const char* datatype;
  uint16_t sensorID;
  float baseValue;
  float variation;
};

const SensorConfig sensors[] = {
  {"TM1000", "K",            "temperature", 1, 25.5f,    0.5f},
  {"HD1000", "watercontent",  "humidity",    2, 60.2f,    0.3f},
  {"PP1000", "hPa",          "pressure",    3, 1013.25f, 0.1f},
  {"NM1000", "nm",           "light",       4, 0.2f,     0.0f},
};
const int NUM_SENSORS = sizeof(sensors) / sizeof(sensors[0]);

const PanoramaServerConfig SERVER_CONFIG = {
  "ESP32-Interface",
  "12345678",
  9000
};

void setup() {
  Serial.begin(115200);

  // onboard LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  panoramaServer.init(SERVER_CONFIG);
  panoramaServer.start();
}

void handleCommand(const PanoramaCommand& command) {
  const String& cmd = command.cmd;

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
  panoramaServer.pollClient();

  // update onboard LED: solid when connected, blink when disconnected
  if (panoramaServer.isClientConnected()) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    unsigned long now = millis();
    if (now - lastBlink >= BLINK_DELAY_MS) {
      lastBlink = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);
    }
  }

  PanoramaCommand command;
  if (panoramaServer.getCommand(command)) {
    handleCommand(command);
  }

  if (sendEnabled && panoramaServer.isClientConnected()) {
    unsigned long now = millis();
    if (now - lastSend >= sampleInterval) {
      lastSend = now;

      int idx = seq % NUM_SENSORS;
      const SensorConfig& s = sensors[idx];
      float value = s.baseValue + (seq % 10) * s.variation;
      unsigned long timestamp = now - startTime;

      PanoramaPacket packet = {
        s.sensor,
        s.dataunit,
        s.datatype,
        s.sensorID,
        value,
        timestamp
      };

      panoramaServer.sendPacket(packet);
      seq++;
    }
  }
}
