#ifndef SECURITY_LAYER_H
#define SECURITY_LAYER_H

#include <memory>
#include <string>
#include <vector>

#include <cose_defines.h>
#include <cose/key.h>

#include "SecurityParams.h"

#define DEFAULT_MASTER_SALT_LEN 16
#define COSE_SERIALIZATION_MAX_OVERHEAD 12
#define COSE_INTERNAL_BUF_SIZE 512

namespace security
{

enum SecurityResult
{
  OK = 0,
  PARAM_ERROR = -1,
  KEY_SIZE_ERROR = -2,
  KEY_ENV_ERROR = -3,
  NONCE_ENV_ERROR = -4,
  NONCE_SIZE_ERROR = -5,
  BUFFER_ERROR = -6,
  CONTEXT_DERIVE_ERROR = -7,
  COMP_ERROR = -8,
  WRAP_ERROR = -100,
  UNWRAP_ERROR = -200,
  INTERNAL_ERROR = -999
};

class SecurityLayer
{
  public:
    SecurityLayer();
    SecurityLayer(const AeadParams &aead_params, KdfAlgorithms kdf, const std::vector<uint8_t> &master_key,
      const std::vector<uint8_t> &master_salt, size_t piv_size, size_t sender_seq_number,
      const std::vector<uint8_t> &sender_id, const std::vector<uint8_t> &receiver_id);

    SecurityResult wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len);
    SecurityResult unwrap(uint8_t* data, size_t data_len, size_t data_max_len, size_t* new_data_len);

  private:
    SecurityResult get_master_key_env(std::vector<uint8_t>& master_key) const;
    SecurityResult get_master_salt_env(std::vector<uint8_t>& master_salt) const;
    SecurityResult generate_nonce();
    SecurityResult build_kdf_info(const std::vector<uint8_t>& id, cose_algo_t alg, const std::string& type, size_t len, uint8_t** out, size_t* out_len);
    SecurityResult derive_key(const std::vector<uint8_t>& ikm, const std::vector<uint8_t>& salt, const std::vector<uint8_t>& id, cose_algo_t algo, std::vector<uint8_t>& key_out, cose_key_t* cose_k_out);
    SecurityResult derive_iv(const std::vector<uint8_t>& ikm, const std::vector<uint8_t>& salt, cose_algo_t alg, std::vector<uint8_t>& iv_out);
    SecurityResult derive_context(const std::vector<uint8_t>& master_key, const std::vector<uint8_t>& master_salt);
    SecurityResult cose_stateless_compress(uint8_t* cose, size_t cose_len, uint8_t** out, size_t* out_len) const;
    SecurityResult cose_stateless_decompress(uint8_t* data, size_t data_len, size_t data_max_len, size_t* out_len) const;

    /* Algorithms to use */
    AeadParams aead_params_;
    KdfAlgorithms kdf_;
    /* ***************** */

    /* Nonce-related fields */
    std::vector<uint8_t> context_iv_{};
    size_t piv_size_;
    std::vector<uint8_t> piv_bytes_{};
    size_t sender_seq_number_;
    /* ******************** */

    /* Key-related fields */
    std::vector<uint8_t> sender_id_{};
    std::vector<uint8_t> sender_key_bytes_{};
    cose_key_t sender_cose_key_{};
    std::vector<uint8_t> receiver_id_{};
    std::vector<uint8_t> receiver_key_bytes_{};
    cose_key_t receiver_cose_key_{};
    /* ****************** */

    uint8_t internal_buf_[COSE_INTERNAL_BUF_SIZE]{};
};

}  // namespace security

#endif
