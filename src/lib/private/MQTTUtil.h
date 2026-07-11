/**
 * @file MQTTUtil.h
 * @author Edward A. Lee
 * @brief Utility functions for MQTT.
 */
#ifndef MQTT_UTIL_H
#define MQTT_UTIL_H
#include <stdint.h>   // Defines int64_t

/**
 * @brief Write the specified data as a sequence of bytes starting at the specified address.
 *
 * This encodes the data in little-endian order (lowest order byte first).
 * @param data The data to write.
 * @param buffer The location to start writing.
 */
void mqtt_encode_int64(int64_t data, unsigned char* buffer);

/**
 * @brief This will swap the order of the bytes if this machine is big endian.
 *
 * @param bytes The address of the start of the sequence of bytes.
 */
int64_t mqtt_extract_int64(unsigned char* bytes);

#endif // MQTT_UTIL_H