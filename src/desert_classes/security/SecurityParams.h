#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <cstdint>

#define AEAD_ALGO_ASCON_128 "Ascon-AEAD128"
#define AEAD_ALGO_ASCON_128_64 "Ascon-AEAD128-64"
#define AEAD_ALGO_ASCON_128_32 "Ascon-AEAD128-32"

#define KDF_ALGO_ASCON_256 "HKDF-Ascon256"

namespace security
{

enum AeadAlgorithms
{
  ASCON_AEAD128,
  ASCON_AEAD128_64,
  ASCON_AEAD128_32,
  A128GCM,
  AEAD_UNKNOWN
};

enum KdfAlgorithms {
  HKDF_ASCON,
  KDF_UNKNOWN
};

class AeadParams
{
  public:
    AeadParams(AeadAlgorithms alg);

    AeadAlgorithms get_alg() const { return alg_; }
    uint16_t get_key_size() const { return key_size_; }
    uint16_t get_nonce_size() const { return nonce_size_; }
    uint16_t get_tag_size() const { return tag_size_; }

  private:
    AeadAlgorithms alg_;
    uint16_t key_size_;
    uint16_t nonce_size_;
    uint16_t tag_size_;
};

}  // namespace security

#endif
