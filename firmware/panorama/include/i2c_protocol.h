#ifndef I2C_PROTOCOL_H
#define I2C_PROTOCOL_H

#include <Arduino.h>
#include <Wire.h>

// Maximum number of I2C sensors to scan for and manage
const uint8_t MAX_I2C_SENSORS = 16; 

// I2C address 0 is reserved for general call, so we start scanning from 1. The upper limit is 127 for 7-bit addressing.
const uint8_t I2C_ADDRESS_START = 1; 

// Note: Some addresses may be reserved by the system or other devices, so the actual number of usable addresses may be less than 127.
const uint8_t I2C_ADDRESS_END = 127; 

/***************************************************************************
 * sets up the I2C interface with the specified SDA and SCL pins and clock speed. Must be called in setup() before using I2C functions.
 * @param sdaPin: GPIO pin number for SDA line
 * @param sclPin: GPIO pin number for SCL line
 * @param clockHz: I2C clock speed in Hz (e.g., 100000 for 100 kHz, 400000 for 400 kHz)
***************************************************************************/
void setupI2C(int sdaPin, int sclPin, uint32_t clockHz);

/***************************************************************************
 * Scans the I2C bus for devices and fills the provided addresses array with found device addresses. Returns the number of devices found.
 * @param addresses: Pointer to an array where found device addresses will be stored
 * @param maxAddresses: Maximum number of addresses that can be stored in the provided array
 * @return Number of I2C devices found on the bus
***************************************************************************/
size_t scanForI2CDevices(uint8_t *addresses, size_t maxAddresses);

/***************************************************************************
 * Reads data from an I2C device at the specified address into the provided buffer. Returns true if the read was successful, false otherwise.
 * @param address: I2C address of the device to read from
 * @param buffer: Pointer to a buffer where the read data will be stored
 * @param len: Number of bytes to read
 * @param timeoutMs: Optional timeout in milliseconds for the I2C read operation (default is 100 ms)
 * @return True if the read was successful, false if there was an error or timeout
***************************************************************************/
bool readFromI2C(uint8_t address, uint8_t *buffer, size_t len, uint32_t timeoutMs = 100);

#endif
