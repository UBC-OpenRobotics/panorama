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

struct UdpSubscriber {
    IPAddress ip;
    uint16_t port;
    bool active;
};

const int MAX_UDP_SUBSCRIBERS = 5;
UdpSubscriber subscribers[MAX_UDP_SUBSCRIBERS];

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

void sendToUdpPeer(IPAddress ip, uint16_t port, const String &line) {
    udp.beginPacket(ip, port);
    udp.print(line);
    udp.endPacket();
}

void sendToAllUdpSubscribers(const String &line) {
    for (int i = 0; i < MAX_UDP_SUBSCRIBERS; i++) {
        if (!subscribers[i].active) {
            continue;
        }
        sendToUdpPeer(subscribers[i].ip, subscribers[i].port, line);
    }
}

bool hasUdpSubscribers() {
    for (int i = 0; i < MAX_UDP_SUBSCRIBERS; i++) {
        if (subscribers[i].active) {
            return true;
        }
    }
    return false;
}

int findUdpSubscriber(IPAddress ip, uint16_t port) {
    for (int i = 0; i < MAX_UDP_SUBSCRIBERS; i++) {
        if (subscribers[i].active &&
            subscribers[i].ip == ip &&
            subscribers[i].port == port) {
            return i;
        }
    }
    return -1;
}

bool addUdpSubscriber(IPAddress ip, uint16_t port) {
    if (findUdpSubscriber(ip, port) >= 0) {
        return true;
    }

    for (int i = 0; i < MAX_UDP_SUBSCRIBERS; i++) {
        if (!subscribers[i].active) {
            subscribers[i].ip = ip;
            subscribers[i].port = port;
            subscribers[i].active = true;
            return true;
        }
    }

    return false;
}

void removeUdpSubscriber(IPAddress ip, uint16_t port) {
    int index = findUdpSubscriber(ip, port);
    if (index < 0) {
        return;
    }

    subscribers[index].active = false;
    subscribers[index].port = 0;
}

void replyToSource(CommandSource src,
                   const String &line,
                   IPAddress remoteIp = IPAddress(),
                   uint16_t remotePort = 0) {
    if (src == SRC_TCP) {
        sendToTcp(line);
    } else {
        sendToUdpPeer(remoteIp, remotePort, line);
    }
}

bool canStreamToActiveTarget() {
    if (activeMode == MODE_TCP) {
        return tcpClient && tcpClient.connected();
    }
    return hasUdpSubscribers();
}

void sendDataLine(const String &line) {
    if (activeMode == MODE_TCP) {
        sendToTcp(line);
    } else {
        sendToAllUdpSubscribers(line);
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

void setMode(TransportMode mode,
             CommandSource src,
             IPAddress remoteIp = IPAddress(),
             uint16_t remotePort = 0) {
    activeMode = mode;
    String ack = "{\"type\":\"ack\",\"cmd\":\"MODE\",\"mode\":\"";
    ack += (mode == MODE_TCP) ? "TCP" : "UDP";
    ack += "\"}\n";
    replyToSource(src, ack, remoteIp, remotePort);
}

void handleCommand(String cmd, CommandSource src, IPAddress remoteIp = IPAddress(), uint16_t remotePort = 0) {
    cmd.trim();
    cmd.toUpperCase();

    if (cmd.startsWith("MODE ")) {
        String mode = cmd.substring(5);
        mode.trim();
        if (mode == "TCP") {
            setMode(MODE_TCP, src, remoteIp, remotePort);
        } else if (mode == "UDP") {
            setMode(MODE_UDP, src, remoteIp, remotePort);
        } else {
            replyToSource(src, "{\"type\":\"error\",\"msg\":\"mode_must_be_tcp_or_udp\"}\n", remoteIp, remotePort);
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
        if (src == SRC_UDP && !addUdpSubscriber(remoteIp, remotePort)) {
            replyToSource(src, "{\"type\":\"error\",\"msg\":\"udp_subscriber_limit_reached\"}\n", remoteIp, remotePort);
            return;
        }
        sendEnabled = true;
        Serial.printf("Streaming STARTED at %.2f Hz via %s\n",
                      1000.0f / sampleIntervalMs,
                      activeMode == MODE_TCP ? "TCP" : "UDP");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"START\"}\n", remoteIp, remotePort);
        return;
    }

    if (cmd == "STOP") {
        if (src == SRC_UDP) {
            removeUdpSubscriber(remoteIp, remotePort);
            sendEnabled = hasUdpSubscribers();
        } else {
            sendEnabled = false;
        }
        Serial.println("Streaming STOPPED");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"STOP\"}\n", remoteIp, remotePort);
        return;
    }

    if (cmd == "END") {
        if (src == SRC_UDP) {
            removeUdpSubscriber(remoteIp, remotePort);
            sendEnabled = hasUdpSubscribers();
        } else {
            sendEnabled = false;
        }
        Serial.println("Session ENDED by client");
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"END\"}\n", remoteIp, remotePort);
        if (src == SRC_TCP && tcpClient && tcpClient.connected()) {
            tcpClient.stop();
        }
        return;
    }

    if (cmd == "RESCAN") {
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"RESCAN\",\"msg\":\"gpio_mode_no_scan\"}\n", remoteIp, remotePort);
        return;
    }

    replyToSource(src, "{\"type\":\"error\",\"msg\":\"unknown_cmd\"}\n", remoteIp, remotePort);
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

    String cmd = String(buffer);
    handleCommand(cmd, SRC_UDP, udp.remoteIP(), udp.remotePort());
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
