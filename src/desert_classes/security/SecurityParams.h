#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <cstdint>

namespace security
{

enum AeadAlgorithms
{
  ASCON_AEAD128,
  ASCON_AEAD128_64,
  ASCON_AEAD128_32,
  A128GCM
};

enum KdfAlgorithms {
  HKDF_ASCON
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
