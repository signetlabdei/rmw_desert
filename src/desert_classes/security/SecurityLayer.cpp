#include <cose.h>
#include <cose/crypto.h>

#include "SecurityLayer.h"

#include <netinet/in.h>

#include "sec_utils.h"

#define KDF_INFO_TYPE_KEY "Key"
#define KDF_INFO_TYPE_IV  "IV"

namespace security
{

static AeadAlgorithms str_to_aead_algo(const std::string& str) {
  if (str == AEAD_ALGO_ASCON_128) {
    return ASCON_AEAD128;
  }
  if (str == AEAD_ALGO_ASCON_128_64) {
    return ASCON_AEAD128_64;
  }
  if (str == AEAD_ALGO_ASCON_128_32) {
    return ASCON_AEAD128_32;
  }

  return AEAD_UNKNOWN;
}

static KdfAlgorithms str_to_kdf_algo(const std::string& str) {
  if (str == KDF_ALGO_ASCON_256) {
    return HKDF_ASCON;
  }

  return KDF_UNKNOWN;
}

static cose_algo_t aead_algo_to_cose_algo(AeadAlgorithms algo)
{
  switch (algo)
  {
    case ASCON_AEAD128:
    case ASCON_AEAD128_64:
    case ASCON_AEAD128_32:
    case A128GCM:
      return COSE_ALGO_A128GCM;
  }

  return COSE_ALGO_NONE;
}

static cose_algo_t kdf_algo_to_cose_algo(KdfAlgorithms algo) {
  switch (algo) {
    // TODO: Change
    case HKDF_ASCON:
      return COSE_ALGO_HMAC256;
  }

  return COSE_ALGO_NONE;
}

static SecurityResult cbor_err_to_sec_err(int err) {
  switch (err) {
    case 0:
      return OK;
    case NANOCBOR_ERR_END:
      return BUFFER_ERROR;
  }

  return INTERNAL_ERROR;
}

SecurityLayer::SecurityLayer()
  : aead_params_(AeadParams(str_to_aead_algo(AEAD_ALGO))),
    kdf_(str_to_kdf_algo(KDF_ALGO)),
    piv_size_(2),
    piv_bytes_(piv_size_),
    sender_seq_number_(0)
{
  if (!decode_hex(SENDER_ID, sender_id_)) {
    throw std::runtime_error("Failed to parse the sender id");
  }

  if (!decode_hex(RECEIVER_ID, receiver_id_)) {
    throw std::runtime_error("Failed to parse the receiver id");
  }

  std::vector<uint8_t> master_key{};
  if (get_master_key_env(master_key) != OK)
  {
    throw std::runtime_error("Failed to parse the encryption key");
  }

  std::vector<uint8_t> master_salt{};
  if (get_master_salt_env(master_salt) != OK)
  {
    master_salt = std::vector<uint8_t>(DEFAULT_MASTER_SALT_LEN, 0);
  }

  auto res = derive_context(master_key, master_salt);
  if (res != OK) {
    throw std::runtime_error("Security context initialization error");
  }
}

SecurityResult SecurityLayer::build_kdf_info(const std::vector<uint8_t>& id, cose_algo_t alg, const std::string& type, size_t len, uint8_t** out, size_t* out_len)
{
  nanocbor_encoder_t info;
  int st = COSE_OK;
  nanocbor_encoder_init(&info, internal_buf_, sizeof(internal_buf_));
  st = nanocbor_fmt_array(&info, 4);
  if (st < 0) {
    return cbor_err_to_sec_err(st);
  }

  st = nanocbor_put_bstr(&info, id.data(), id.size());
  if (st < 0) {
    return cbor_err_to_sec_err(st);
  }

  st = nanocbor_fmt_int(&info, alg);
  if (st < 0) {
    return cbor_err_to_sec_err(st);
  }

  st = nanocbor_put_tstrn(&info, type.data(), type.size());
  if (st < 0) {
    return cbor_err_to_sec_err(st);
  }

  st = nanocbor_fmt_uint(&info, len);
  if (st < 0) {
    return cbor_err_to_sec_err(st);
  }

  *out = internal_buf_;
  *out_len = nanocbor_encoded_len(&info);

  return OK;
}

SecurityResult SecurityLayer::derive_key(const std::vector<uint8_t>& ikm, const std::vector<uint8_t>& master_salt, const std::vector<uint8_t>& id, cose_algo_t alg, std::vector<uint8_t>& key_out, cose_key_t* cose_k_out)
{
  uint8_t* info_ptr;
  size_t info_len;
  auto info_res = build_kdf_info(id,
    alg,
    KDF_INFO_TYPE_KEY,
    aead_params_.get_key_size(),
    &info_ptr,
    &info_len);
  if (info_res != OK) {
    return info_res;
  }

  key_out.resize(aead_params_.get_key_size());
  auto kdf_res = cose_crypto_hkdf_derive(master_salt.data(),
    master_salt.size(),
    ikm.data(),
    aead_params_.get_key_size(),
    info_ptr,
    info_len,
    key_out.data(),
    aead_params_.get_key_size(),
    kdf_algo_to_cose_algo(kdf_));
  if (kdf_res != COSE_OK) {
    return CONTEXT_DERIVE_ERROR;
  }

  // static uint8_t kid[] = "rmw_desert_sender_key";
  cose_key_init(cose_k_out);
  // cose_key_set_kid(&cose_key_, kid, sizeof(kid) - 1);
  cose_key_set_keys(cose_k_out, COSE_EC_NONE, alg, nullptr, nullptr, key_out.data());

  return OK;
}

SecurityResult SecurityLayer::derive_iv(const std::vector<uint8_t>& ikm, const std::vector<uint8_t>& master_salt, cose_algo_t alg, std::vector<uint8_t>& iv_out)
{
  uint8_t* info_ptr;
  size_t info_len;
  auto info_res = build_kdf_info({},
    alg,
    KDF_INFO_TYPE_IV,
    aead_params_.get_nonce_size(),
    &info_ptr,
    &info_len);
  if (info_res != OK) {
    return info_res;
  }

  iv_out.resize(aead_params_.get_nonce_size());
  auto kdf_res = cose_crypto_hkdf_derive(master_salt.data(),
    master_salt.size(),
    ikm.data(),
    aead_params_.get_key_size(),
    info_ptr,
    info_len,
    iv_out.data(),
    aead_params_.get_nonce_size(),
    kdf_algo_to_cose_algo(kdf_));
  if (kdf_res != COSE_OK) {
    return CONTEXT_DERIVE_ERROR;
  }

  return OK;
}

SecurityResult SecurityLayer::get_master_key_env(std::vector<uint8_t>& master_key) const
{
  const char* env_value = std::getenv("MASTER_SECRET_KEY");
  if (env_value == nullptr || !decode_hex(env_value, master_key))
  {
    printf("CRITICAL: Master Secret Key is invalid\n");
    printf("Please fill the MASTER_SECRET_KEY environment variable with stream of hexadecimal bytes\n");
    return KEY_ENV_ERROR;
  }
  if (master_key.size() < aead_params_.get_key_size())
  {
    printf("CRITICAL: the master secret key size does not match the required suite\n");
    printf("Please set a MASTER_SECRET_KEY of %d bytes\n", aead_params_.get_key_size());
    return KEY_SIZE_ERROR;
  }

  return OK;
}

SecurityResult SecurityLayer::get_master_salt_env(std::vector<uint8_t>& master_salt) const
{
  const char* env_value = std::getenv("MASTER_SALT");
  if (env_value == nullptr || !decode_hex(env_value, master_salt))
  {
    printf("CRITICAL: Master Salt is invalid\n");
    printf("Please fill the MASTER_SALT environment variable with stream of hexadecimal bytes\n");
    return NONCE_ENV_ERROR;
  }

  return OK;
}

/* Generating Partial Initialization Vector instead of full nonce */
SecurityResult SecurityLayer::generate_nonce()
{
  uint32_t seq = htonl(sender_seq_number_);
  memcpy(piv_bytes_.data(), &seq, MIN(sizeof(seq), piv_size_));
  return OK;
}

SecurityResult SecurityLayer::derive_context(const std::vector<uint8_t>& master_key, const std::vector<uint8_t>& master_salt)
{
  // Master Secret and Master Salt have to be set already
  cose_algo_t algo = aead_algo_to_cose_algo(aead_params_.get_alg());
  if (algo == COSE_ALGO_NONE) {
    return PARAM_ERROR;
  }

  // Derive Sender Key
  auto res = derive_key(master_key, master_salt, sender_id_, algo, sender_key_bytes_, &sender_cose_key_);
  if (res != OK) {
    return res;
  }
  // Derive Receiver Key
  res = derive_key(master_key, master_salt, receiver_id_, algo, receiver_key_bytes_, &receiver_cose_key_);
  if (res != OK) {
    return res;
  }

  // Derive Context IV
  res = derive_iv(master_key, master_salt, algo, context_iv_);
  return res;
}

SecurityResult SecurityLayer::wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len)
{
  auto nonce_st = generate_nonce();
  if (nonce_st != OK)
  {
    return nonce_st;
  }
  std::vector<uint8_t> payload(data, data + data_len);

  cose_encrypt_t crypt;
  cose_encrypt_init(&crypt, COSE_FLAGS_ENCRYPT0 | COSE_FLAGS_UNTAGGED);

  cose_hdr_t piv = {
    .next = nullptr,
    .key = COSE_HDR_PARTIALIV,
    .len = piv_size_,
    .v = {
      .data = piv_bytes_.data()
    },
    .type = COSE_HDR_TYPE_BSTR
  };
  cose_hdr_insert(&crypt.hdrs.unprot, &piv);

  cose_encrypt_add_recipient(&crypt, &sender_cose_key_);
  cose_encrypt_set_payload(&crypt, payload.data(), payload.size());
  cose_encrypt_set_algo(&crypt, COSE_ALGO_DIRECT);
  COSE_ssize_t len = cose_encrypt_encode(&crypt, data, data_max_len, context_iv_.data(), cose_ptr);
  if (len <= 0)
  {
    return WRAP_ERROR;
  }
  *cose_len = len;
  sender_seq_number_++;
  return OK;
}

SecurityResult SecurityLayer::unwrap(uint8_t* data, size_t data_len, size_t* new_data_len)
{
  cose_encrypt_dec_t decrypt;
  if (cose_encrypt_decode(&decrypt, data, data_len) != COSE_OK)
  {
    return UNWRAP_ERROR;
  }

  std::vector<uint8_t> plaintext(data_len);
  if (cose_encrypt_decrypt(&decrypt, nullptr, &receiver_cose_key_, internal_buf_, sizeof(internal_buf_), plaintext.data(), new_data_len))
  {
    return UNWRAP_ERROR;
  }
  memcpy(data, plaintext.data(), *new_data_len);

  return OK;
}

}  // namespace security
