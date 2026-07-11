/**
 * @file MQTTUtil.c
 * @author Edward A. Lee
 * @brief Utility functions for MQTT.
 */
#include "MQTTUtil.h"
#include <stddef.h>   // Defines size_t
#include <stdbool.h>  // Defines bool
#include <string.h>   // Defines memcpy()

/**
 * @brief Return true (1) if the host is big endian. Otherwise, return false.
 */
static bool host_is_big_endian() {
  uint32_t x = 1;
  return (*(unsigned char*)&x == 0);
}

/**
 * @brief If this host is little endian, then reverse the order of the bytes of the argument.
 *
 * Otherwise, return the argument unchanged.
 * This can be used to convert the argument to network order (big endian) and then back again.
 * Network transmissions, by convention, are big endian, meaning that the high-order byte is sent first.
 * But many platforms, including my Mac, are little endian, meaning that the low-order byte is first in memory.
 * @param src The argument to convert.
 */
static int64_t swap_bytes_if_big_endian_int64(int64_t src) {
  union {
    int64_t ull;
    unsigned char c[sizeof(int64_t)];
  } x;
  if (!host_is_big_endian()) return src;
  x.ull = src;
  unsigned char c;
  // Swap bytes.
  c = x.c[0];
  x.c[0] = x.c[7];
  x.c[7] = c;
  c = x.c[1];
  x.c[1] = x.c[6];
  x.c[6] = c;
  c = x.c[2];
  x.c[2] = x.c[5];
  x.c[5] = c;
  c = x.c[3];
  x.c[3] = x.c[4];
  x.c[4] = c;
  return x.ull;
}

void encode_int64(int64_t data, unsigned char* buffer) {
  // This strategy is fairly brute force, but it avoids potential
  // alignment problems.
  int shift = 0;
  for (size_t i = 0; i < sizeof(int64_t); i++) {
    buffer[i] = (unsigned char)((data & (0xffLL << shift)) >> shift);
    shift += 8;
  }
}

int64_t extract_int64(unsigned char* bytes) {
  // Use memcpy to prevent possible alignment problems on some processors.
  union {
    int64_t ull;
    unsigned char c[sizeof(int64_t)];
  } result;
  memcpy(&result.c, bytes, sizeof(int64_t));
  return swap_bytes_if_big_endian_int64(result.ull);
}
