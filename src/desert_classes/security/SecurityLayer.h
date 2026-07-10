#ifndef SECURITY_LAYER_H
#define SECURITY_LAYER_H

#include <memory>
#include <string>
#include <vector>

#include "AeadParams.h"

#define COSE_INTERNAL_BUF_SIZE 512

namespace security
{

enum SecurityResult
{
  OK, KEY_SIZE_ERROR, KEY_ENV_ERROR, WRAP_ERROR, UNWRAP_ERROR
};

class SecurityLayer
{
  public:
    SecurityLayer();

    SecurityResult wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len);
    SecurityResult unwrap(uint8_t* data, size_t data_len, size_t* new_data_len);

  private:
    SecurityResult get_key();
    SecurityResult generate_nonce(uint8_t* nonce_buf);

    AeadParams aead_params_;
    std::vector<uint8_t> key_bytes_{};
    uint8_t internal_buf_[COSE_INTERNAL_BUF_SIZE]{};
};

}  // namespace security

#endif
