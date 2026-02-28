#include "i2c_protocol.h"

void setupI2C(int sdaPin, int sclPin, uint32_t clockHz) {
    Wire.begin(sdaPin, sclPin, clockHz);
    Serial.printf("I2C started on SDA=%d SCL=%d @ %lu Hz\n", sdaPin, sclPin, clockHz);
}

size_t scanForI2CDevices(uint8_t *addresses, size_t maxAddresses) {
    size_t sensorCount = 0;

    for (uint8_t address = I2C_ADDRESS_START; address < I2C_ADDRESS_END; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            if (sensorCount < maxAddresses) {
                addresses[sensorCount] = address;
            }
            sensorCount++;
            Serial.printf("I2C device found at 0x%02X\n", address);
        }
    }

    if (sensorCount == 0) {
        Serial.println("No I2C devices found");
    } else {
        Serial.printf("Total I2C devices found: %u\n", (unsigned int)sensorCount);
    }

    if (sensorCount > maxAddresses) {
        Serial.printf("Tracking first %u devices only\n", (unsigned int)maxAddresses);
        sensorCount = maxAddresses;
    }

    return sensorCount;
}

bool readFromI2C(uint8_t address, uint8_t *buffer, size_t len, uint32_t timeoutMs) {
    int requested = Wire.requestFrom((int)address, (int)len);
    if (requested != (int)len) {
        while (Wire.available()) {
            (void)Wire.read();
        }
        return false;
    }

    size_t index = 0;
    unsigned long startTime = millis();
    while (index < len) {
        if (Wire.available()) {
            buffer[index++] = Wire.read();
            continue;
        }
        if ((millis() - startTime) > timeoutMs) {
            return false;
        }
    }
    return true;
}
