
#include <cose.h>

#include "SecurityConfig.h"
#include "SecurityLayer.h"
#include "sec_utils.h"

namespace rmw_desert::security {

    namespace {
        cose_algo_t aead_algo_to_cose_algo(AeadAlgorithms algo) {
            switch (algo) {
                case ASCON_AEAD128:
                case ASCON_AEAD128_64:
                case ASCON_AEAD128_32:
                case A128GCM:
                    return COSE_ALGO_A128GCM;
            }

            return COSE_ALGO_A128GCM;
        }
    }

    void KeyProvider::set_key_size(size_t k_size) {
        k_size_ = k_size;
    }

    size_t KeyProvider::get_key_size() {
        return k_size_;
    }

    EnvKeyProvider::EnvKeyProvider(std::string env_name)
        : env_name_(std::move(env_name)) {
    }

    SecurityResult EnvKeyProvider::provide(const uint8_t **key_buf) {
        if (key_bytes_.empty()) {
            const char* env_value = std::getenv(env_name_.c_str());
            if (env_value == nullptr || !decode_hex(env_value, key_bytes_)) {
                *key_buf = nullptr;
                return KEY_ENV_ERROR;
            }
            if (key_bytes_.size() < k_size_) {
                return KEY_SIZE_ERROR;
            }
        }

        *key_buf = key_bytes_.data();
        return OK;
    }

    void NonceGenerator::set_nonce_size(size_t n_size) {
        n_size_ = n_size;
    }

    size_t NonceGenerator::get_nonce_size() {
        return n_size_;
    }

    SecurityResult DefaultNVMNonceGenerator::generate(uint8_t* nonce_buf) {
        // TODO: Implement
        memset(nonce_buf, 0, n_size_);
        return OK;
    }

    CoseSecurityLayer::CoseSecurityLayer(std::shared_ptr<AeadParams> params,
        std::shared_ptr<KeyProvider> key_provider,
        std::shared_ptr<NonceGenerator> nonce_gen_v)
        : aead_params_(std::move(params)),
            key_provider_(std::move(key_provider)),
            nonce_gen_(std::move(nonce_gen_v)) {
        key_provider_->set_key_size(aead_params_->get_key_size());
        nonce_gen_->set_nonce_size(aead_params_->get_nonce_size());
    }

    SecurityResult CoseSecurityLayer::wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len) {
        const uint8_t* key_bytes;
        auto key_st = key_provider_->provide(&key_bytes);
        if (key_st != OK) {
            return key_st;
        }

        cose_algo_t algo = aead_algo_to_cose_algo(aead_params_->get_alg());
        std::vector<uint8_t> nonce_bytes(nonce_gen_->get_nonce_size());
        auto nonce_st = nonce_gen_->generate(nonce_bytes.data());
        if (nonce_st != OK) {
            return nonce_st;
        }
        static uint8_t kid[] = "rmw_desert_key";
        // uint8_t payload[data_len];
        std::vector<uint8_t> payload(data, data + data_len);
        // memcpy(payload, data, data_len);

        cose_key_t key;
        cose_key_init(&key);
        cose_key_set_kid(&key, kid, sizeof(kid) - 1);
        cose_key_set_keys(&key, COSE_EC_NONE, algo, nullptr, nullptr, const_cast<uint8_t *>(key_bytes));

        cose_encrypt_t crypt;
        cose_encrypt_init(&crypt, COSE_FLAGS_ENCRYPT0 | COSE_FLAGS_UNTAGGED);
        cose_encrypt_add_recipient(&crypt, &key);
        cose_encrypt_set_payload(&crypt, payload.data(), payload.size());
        cose_encrypt_set_algo(&crypt, COSE_ALGO_DIRECT);
        COSE_ssize_t len = cose_encrypt_encode(&crypt, data, data_max_len, nonce_bytes.data(), cose_ptr);
        if (len <= 0) {
            return WRAP_ERROR;
        }
        *cose_len = len;

        return OK;
    }

    SecurityResult CoseSecurityLayer::unwrap(uint8_t* data, size_t data_len, size_t* new_data_len) {
        const uint8_t* key_bytes;
        auto key_st = key_provider_->provide(&key_bytes);
        if (key_st != OK) {
            return key_st;
        }

        cose_encrypt_dec_t decrypt;
        if (cose_encrypt_decode(&decrypt, data, data_len) != COSE_OK) {
            return UNWRAP_ERROR;
        }

        cose_algo_t algo = aead_algo_to_cose_algo(aead_params_->get_alg());
        static uint8_t kid[] = "rmw_desert_key";

        cose_key_t key;
        cose_key_init(&key);
        cose_key_set_kid(&key, kid, sizeof(kid) - 1);
        cose_key_set_keys(&key, COSE_EC_NONE, algo, nullptr, nullptr, const_cast<uint8_t *>(key_bytes));

        std::vector<uint8_t> plaintext(data_len);
        if (cose_encrypt_decrypt(&decrypt, nullptr, &key, internal_buf_, sizeof(internal_buf_), plaintext.data(), new_data_len)) {
            return UNWRAP_ERROR;
        }
        memcpy(data, plaintext.data(), *new_data_len);

        return OK;
    }
}
