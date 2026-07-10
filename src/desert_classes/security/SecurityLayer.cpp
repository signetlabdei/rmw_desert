#include <cose.h>

#include "SecurityConfig.h"
#include "SecurityLayer.h"
#include "sec_utils.h"

namespace security
{

cose_algo_t aead_algo_to_cose_algo(AeadAlgorithms algo)
{
  switch (algo)
  {
    case ASCON_AEAD128:
    case ASCON_AEAD128_64:
    case ASCON_AEAD128_32:
    case A128GCM:
      return COSE_ALGO_A128GCM;
  }
  
  return COSE_ALGO_A128GCM;
}

SecurityLayer::SecurityLayer()
      : aead_params_(AeadParams(A128GCM))
{
  if (get_key() != OK)
  {
    throw std::runtime_error("Failed to get the symmetric key");
  }
}

SecurityResult SecurityLayer::generate_nonce(uint8_t* nonce_buf)
{
  // TODO: Implement
  memset(nonce_buf, 0, aead_params_.get_nonce_size());
  return OK;
}

SecurityResult SecurityLayer::get_key()
{
  if (key_bytes_.empty())
  {
    const char* env_value = std::getenv("MASTER_SECRET_KEY");
    if (env_value == nullptr || !decode_hex(env_value, key_bytes_))
    {
      return KEY_ENV_ERROR;
    }
    if (key_bytes_.size() < aead_params_.get_key_size())
    {
      return KEY_SIZE_ERROR;
    }
  }

  return OK;
}

SecurityResult SecurityLayer::wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len)
{
  cose_algo_t algo = aead_algo_to_cose_algo(aead_params_.get_alg());
  std::vector<uint8_t> nonce_bytes(aead_params_.get_nonce_size());
  auto nonce_st = generate_nonce(nonce_bytes.data());
  if (nonce_st != OK)
  {
    return nonce_st;
  }
  static uint8_t kid[] = "rmw_desert_key";
  // uint8_t payload[data_len];
  std::vector<uint8_t> payload(data, data + data_len);
  // memcpy(payload, data, data_len);

  cose_key_t key;
  cose_key_init(&key);
  cose_key_set_kid(&key, kid, sizeof(kid) - 1);
  cose_key_set_keys(&key, COSE_EC_NONE, algo, nullptr, nullptr, const_cast<uint8_t *>(key_bytes_.data()));

  cose_encrypt_t crypt;
  cose_encrypt_init(&crypt, COSE_FLAGS_ENCRYPT0 | COSE_FLAGS_UNTAGGED);
  cose_encrypt_add_recipient(&crypt, &key);
  cose_encrypt_set_payload(&crypt, payload.data(), payload.size());
  cose_encrypt_set_algo(&crypt, COSE_ALGO_DIRECT);
  COSE_ssize_t len = cose_encrypt_encode(&crypt, data, data_max_len, nonce_bytes.data(), cose_ptr);
  if (len <= 0)
  {
    return WRAP_ERROR;
  }
  *cose_len = len;

  return OK;
}

SecurityResult SecurityLayer::unwrap(uint8_t* data, size_t data_len, size_t* new_data_len)
{
  cose_encrypt_dec_t decrypt;
  if (cose_encrypt_decode(&decrypt, data, data_len) != COSE_OK)
  {
    return UNWRAP_ERROR;
  }

  cose_algo_t algo = aead_algo_to_cose_algo(aead_params_.get_alg());
  static uint8_t kid[] = "rmw_desert_key";

  cose_key_t key;
  cose_key_init(&key);
  cose_key_set_kid(&key, kid, sizeof(kid) - 1);
  cose_key_set_keys(&key, COSE_EC_NONE, algo, nullptr, nullptr, const_cast<uint8_t *>(key_bytes_.data()));

  std::vector<uint8_t> plaintext(data_len);
  if (cose_encrypt_decrypt(&decrypt, nullptr, &key, internal_buf_, sizeof(internal_buf_), plaintext.data(), new_data_len))
  {
    return UNWRAP_ERROR;
  }
  memcpy(data, plaintext.data(), *new_data_len);

  return OK;
}

}  // namespace security
