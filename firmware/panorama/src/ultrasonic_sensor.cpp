#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

// Wi-Fi Access Point credentials
const char *SSID = "ESP32-Interface";
const char *PASS = "12345678";
const uint16_t PORT = 9000;

WiFiServer tcpServer(PORT);
WiFiClient tcpClient;
WiFiUDP udp;

enum TransportMode {
    MODE_TCP,
    MODE_UDP
};

TransportMode activeMode = MODE_TCP;

IPAddress udpPeerIp;
uint16_t udpPeerPort = 0;
bool udpPeerKnown = false;

// Stream control
bool sendEnabled = false;
unsigned long sampleIntervalMs = 1000;
unsigned long lastSendMs = 0;

// GPIO ultrasonic configuration (adjust pins for your wiring)
const uint8_t ULTRASONIC_TRIG_PIN = 5;
const uint8_t ULTRASONIC_ECHO_PIN = 18;
const unsigned long ECHO_TIMEOUT_US = 30000;

const uint16_t SENSOR_ID = 2;
const char *SENSOR_NAME = "ultrasonic";

enum CommandSource {
    SRC_TCP,
    SRC_UDP
};

void sendToTcp(const String &line) {
    if (tcpClient && tcpClient.connected()) {
        tcpClient.print(line);
    }
}

void sendToUdp(const String &line) {
    if (!udpPeerKnown) {
        return;
    }
    udp.beginPacket(udpPeerIp, udpPeerPort);
    udp.print(line);
    udp.endPacket();
}

void replyToSource(CommandSource src, const String &line) {
    if (src == SRC_TCP) {
        sendToTcp(line);
    } else {
        sendToUdp(line);
    }
}

bool canStreamToActiveTarget() {
    if (activeMode == MODE_TCP) {
        return tcpClient && tcpClient.connected();
    }
    return udpPeerKnown;
}

void sendDataLine(const String &line) {
    if (activeMode == MODE_TCP) {
        sendToTcp(line);
    } else {
        sendToUdp(line);
    }
}

void setupAccessPoint() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASS);
    IPAddress ip = WiFi.softAPIP();
    Serial.printf("AP started: %s (%s)\n", SSID, ip.toString().c_str());
}

void setupUltrasonicGpio() {
    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    Serial.printf("Ultrasonic GPIO configured TRIG=%u ECHO=%u\n",
                  (unsigned int)ULTRASONIC_TRIG_PIN,
                  (unsigned int)ULTRASONIC_ECHO_PIN);
}

bool readDistanceCmFromGpio(float &distanceCm) {
    // Send 10us trigger pulse.
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

    unsigned long pulseWidthUs = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
    if (pulseWidthUs == 0) {
        return false;
    }

    // Speed of sound: 0.0343 cm/us, divide by 2 for round trip.
    distanceCm = (pulseWidthUs * 0.0343f) / 2.0f;
    return true;
}

void setMode(TransportMode mode, CommandSource src) {
    activeMode = mode;
    String ack = "{\"type\":\"ack\",\"cmd\":\"MODE\",\"mode\":\"";
    ack += (mode == MODE_TCP) ? "TCP" : "UDP";
    ack += "\"}\n";
    replyToSource(src, ack);
}

void handleCommand(String cmd, CommandSource src) {
    cmd.trim();
    cmd.toUpperCase();

    if (cmd.startsWith("MODE ")) {
        String mode = cmd.substring(5);
        mode.trim();
        if (mode == "TCP") {
            setMode(MODE_TCP, src);
        } else if (mode == "UDP") {
            setMode(MODE_UDP, src);
        } else {
            replyToSource(src, "{\"type\":\"error\",\"msg\":\"mode_must_be_tcp_or_udp\"}\n");
        }
        return;
    }

    if (cmd.startsWith("START")) {
        int spaceIdx = cmd.indexOf(' ');
        if (spaceIdx > 0) {
            float freqHz = cmd.substring(spaceIdx + 1).toFloat();
            if (freqHz > 0.0f) {
                sampleIntervalMs = (unsigned long)(1000.0f / freqHz);
            }
        }
        sendEnabled = true;
        Serial.printf("Streaming STARTED at %.2f Hz via %s\n",
                      1000.0f / sampleIntervalMs,
                      activeMode == MODE_TCP ? "TCP" : "UDP");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"START\"}\n");
        return;
    }

    if (cmd == "STOP") {
        sendEnabled = false;
        Serial.println("Streaming STOPPED");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"STOP\"}\n");
        return;
    }

    if (cmd == "END") {
        sendEnabled = false;
        Serial.println("Session ENDED by client");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"END\"}\n");
        if (src == SRC_TCP && tcpClient && tcpClient.connected()) {
            tcpClient.stop();
        }
        if (src == SRC_UDP) {
            udpPeerKnown = false;
        }
        return;
    }

    if (cmd == "RESCAN") {
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"RESCAN\",\"msg\":\"gpio_mode_no_scan\"}\n");
        return;
    }

    replyToSource(src, "{\"type\":\"error\",\"msg\":\"unknown_cmd\"}\n");
}

void setup() {
    Serial.begin(115200);
    setupAccessPoint();

    tcpServer.begin();
    tcpServer.setNoDelay(true);
    udp.begin(PORT);
    Serial.printf("TCP server + UDP listener started on port %u\n", (unsigned int)PORT);

    setupUltrasonicGpio();
}

void handleTcp() {
    WiFiClient newClient = tcpServer.available();
    if (newClient) {
        if (tcpClient && tcpClient.connected()) {
            tcpClient.stop();
        }
        tcpClient = newClient;
        sendToTcp("{\"type\":\"status\",\"msg\":\"connected\",\"transport\":\"TCP\"}\n");
        Serial.println("TCP client connected");
    }

    if (tcpClient && tcpClient.connected() && tcpClient.available()) {
        String cmd = tcpClient.readStringUntil('\n');
        handleCommand(cmd, SRC_TCP);
    }
}

void handleUdp() {
    int packetSize = udp.parsePacket();
    if (packetSize <= 0) {
        return;
    }

    char buffer[256];
    int len = udp.read(buffer, sizeof(buffer) - 1);
    if (len < 0) {
        return;
    }
    buffer[len] = '\0';

    udpPeerIp = udp.remoteIP();
    udpPeerPort = udp.remotePort();
    udpPeerKnown = true;

    String cmd = String(buffer);
    handleCommand(cmd, SRC_UDP);
}

void maybeStreamData() {
    if (!(sendEnabled && canStreamToActiveTarget())) {
        return;
    }

    unsigned long now = millis();
    if (now - lastSendMs < sampleIntervalMs) {
        return;
    }
    lastSendMs = now;

    float distanceCm = 0.0f;
    bool ok = readDistanceCmFromGpio(distanceCm);

    String json = "{";
    json += "\"sensor\":\"" + String(SENSOR_NAME) + "\",";
    json += "\"sensor_id\":" + String(SENSOR_ID) + ",";
    json += "\"transport\":\"" + String(activeMode == MODE_TCP ? "TCP" : "UDP") + "\",";
    json += "\"timestamp_ms\":" + String(now) + ",";
    json += "\"ok\":" + String(ok ? "true" : "false");
    if (ok) {
        json += ",\"value_cm\":" + String(distanceCm, 2);
    }
    json += "}\n";

    sendDataLine(json);
}

void loop() {
    handleTcp();
    handleUdp();
    maybeStreamData();
}
