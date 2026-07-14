#include "SecurityParams.h"

#include <stdexcept>

namespace security
{

AeadParams::AeadParams(AeadAlgorithms alg)
      : alg_(alg)
{
  switch (alg_)
  {
    case ASCON_AEAD128:
      key_size_ = 16;
      nonce_size_ = 16;
      tag_size_ = 16;
      break;
    case ASCON_AEAD128_64:
      key_size_ = 16;
      nonce_size_ = 16;
      tag_size_ = 8;
      break;
    case ASCON_AEAD128_32:
      key_size_ = 16;
      nonce_size_ = 16;
      tag_size_ = 4;
      break;
    case A128GCM:
      key_size_ = 16;
      nonce_size_ = 12;
      tag_size_ = 16;
      break;
    default:
      throw std::runtime_error("Invalid security profile");
  }
}

}  // namespace security

