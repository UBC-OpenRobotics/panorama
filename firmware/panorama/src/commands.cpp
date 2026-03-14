#include <Arduino.h>
#include "commands.h"

void handleCommand(String cmd) {
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "PING") {
        // reply
        return;
    }
}

if (cmd == "PING") {
    replyToSource(src, "{\"type\":\"pong\"}\n");
    return;
}

if (cmd == "GET_STATUS") {
    String json = "{";
    json += "\"type\":\"status\",";
    json += "\"streaming\":" + String(sendEnabled ? "true" : "false") + ",";
    json += "\"mode\":\"" + String(activeMode == MODE_TCP ? "TCP" : "UDP") + "\",";
    json += "\"rate_hz\":" + String(1000.0f / sampleIntervalMs, 2);
    json += "}\n";
    replyToSource(src, json);
    return;
}

if (cmd == "GET_SENSOR_DATA") {
    float distanceCm = 0.0f;
    bool ok = readDistanceCmFromGpio(distanceCm);

    String json = "{";
    json += "\"sensor\":\"" + String(SENSOR_NAME) + "\",";
    json += "\"sensor_id\":" + String(SENSOR_ID) + ",";
    json += "\"ok\":" + String(ok ? "true" : "false");
    if (ok) {
        json += ",\"value_cm\":" + String(distanceCm, 2);
    }
    json += "}\n";
    replyToSource(src, json);
    return;
}

if (cmd.startsWith("GET_SENSOR ")) {
    int sensorId = cmd.substring(11).toInt();
    if (sensorId == SENSOR_ID) {
        // same body as GET_SENSOR_DATA
    } else {
        replyToSource(src, "{\"type\":\"error\",\"msg\":\"sensor_not_found\"}\n");
    }
    return;
}

if (cmd == "START_STREAM") {
    sendEnabled = true;
    replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"START_STREAM\"}\n");
    return;
}

if (cmd == "STOP_STREAM") {
    sendEnabled = false;
    replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"STOP_STREAM\"}\n");
    return;
}

if (cmd.startsWith("SET_RATE ")) {
    float hz = cmd.substring(9).toFloat();
    if (hz > 0.0f) {
        sampleIntervalMs = (unsigned long)(1000.0f / hz);
        replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"SET_RATE\"}\n");
    } else {
        replyToSource(src, "{\"type\":\"error\",\"msg\":\"invalid_rate\"}\n");
    }
    return;
}

if (cmd.startsWith("SET_MODE ")) {
    String mode = cmd.substring(9);
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

if (cmd == "START_SESSION") {
    replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"START_SESSION\"}\n");
    return;
}

if (cmd == "END_SESSION") {
    sendEnabled = false;
    replyToSource(src, "{\"type\":\"ack\",\"cmd\":\"END_SESSION\"}\n");
    return;
}
