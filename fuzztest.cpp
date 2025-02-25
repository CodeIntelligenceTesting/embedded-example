#include <assert.h>

#include <cifuzz/cifuzz.h>
#include <fuzzer/FuzzedDataProvider.h>

extern "C" {
#include "crypto_1.h"
}

FUZZ_TEST_SETUP() {
}

FUZZ_TEST(const uint8_t *data, size_t size) {
  FuzzedDataProvider fuzzed_data(data, size);

  // Load key
  crypto_key crypto_key;
  std::vector<uint8_t> key_data = fuzzed_data.ConsumeBytes<uint8_t>(KEY_LENGTH);
  std::copy(key_data.begin(), key_data.end(), crypto_key.key);
  crypto_set_key(crypto_key);

  // Set nonce
  crypto_nonce crypto_nonce;
  std::vector<uint8_t> key_nonce_data = fuzzed_data.ConsumeBytes<uint8_t>(KEY_LENGTH);
  std::copy(key_nonce_data.begin(), key_nonce_data.end(), crypto_nonce.nonce);
  crypto_set_nonce(crypto_nonce);

  // Verify Hmac key
  int message_len = fuzzed_data.ConsumeIntegralInRange<int>(0, 1024);
  std::vector<uint8_t> message = fuzzed_data.ConsumeBytes<uint8_t>(message_len);

  crypto_hmac hmac;
  std::vector<uint8_t> hmac_data = fuzzed_data.ConsumeBytes<uint8_t>(sizeof(hmac.hmac));
  std::copy(hmac_data.begin(), hmac_data.end(), hmac.hmac);

  crypto_verify_hmac(message.data(), message.size(), &hmac);
}
