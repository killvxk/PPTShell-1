#pragma once

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

void hmac_sha256_get(uint8_t digest[32],
					 const uint8_t *message, int message_length,
					 const uint8_t *key, int key_length);