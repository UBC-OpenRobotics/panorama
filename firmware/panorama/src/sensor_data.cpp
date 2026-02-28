#include "sensor_data.h"

String sensorDataToJson(const SensorData &data) {
    String json = "{";
    json += "\"sensor\":\"" + String(data.sensorName) + "\",";
    json += "\"sensor_id\":" + String(data.sensorId) + ",";
    json += "\"timestamp_ms\":" + String(data.timestampMs) + ",";
    json += "\"units\":\"" + String(data.units) + "\",";
    json += "\"value\":" + String(data.value, 2);
    json += "}\n";
    return json;
}
