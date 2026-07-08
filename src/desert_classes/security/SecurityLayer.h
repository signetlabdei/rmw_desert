#ifndef SECURITY_LAYER_H
#define SECURITY_LAYER_H

// #include <cstdint>
// #include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "SecurityConfig.h"

#define COSE_INTERNAL_BUF_SIZE 512

namespace rmw_desert::security {

    class SecurityLayer;

    extern const std::unique_ptr<SecurityLayer> g_sec_layer;

    enum SecurityResult {
        OK, KEY_SIZE_ERROR, KEY_ENV_ERROR, WRAP_ERROR, UNWRAP_ERROR
    };

    class NonceGenerator {
    public:
        virtual ~NonceGenerator() = default;

        virtual void set_nonce_size(size_t n_size);
        virtual size_t get_nonce_size();
        virtual SecurityResult generate(uint8_t* nonce_buf) = 0;
    protected:
        size_t n_size_{16};
    };

    class DefaultNVMNonceGenerator : public NonceGenerator {
    public:
        SecurityResult generate(uint8_t* nonce_buf) override;

    };

    class KeyProvider {
    public:
        // explicit KeyProvider(size_t k_size);
        virtual ~KeyProvider() = default;

        virtual void set_key_size(size_t k_size);
        virtual size_t get_key_size();
        virtual SecurityResult provide(const uint8_t **key_buf) = 0;

    protected:
        size_t k_size_{16};
    };

    class EnvKeyProvider : public KeyProvider {
    public:
        EnvKeyProvider(std::string env_name);
        // EnvKeyProvider(size_t k_size, std::string env_name);

        SecurityResult provide(const uint8_t **key_buf) override;

    private:
        std::string env_name_;
        std::vector<uint8_t> key_bytes_{};
    };

    class SecurityLayer {
    public:
        virtual ~SecurityLayer() = default;

        virtual SecurityResult wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len) = 0;
        virtual SecurityResult unwrap(uint8_t* data, size_t data_len, size_t* new_data_len) = 0;
    };

    class CoseSecurityLayer : public SecurityLayer {
    public:
        explicit CoseSecurityLayer(std::shared_ptr<AeadParams> params,
            std::shared_ptr<KeyProvider> key_provider,
            std::shared_ptr<NonceGenerator> nonce_gen);

        SecurityResult wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len) override;
        SecurityResult unwrap(uint8_t* data, size_t data_len, size_t* new_data_len) override;

    private:
        std::shared_ptr<AeadParams> aead_params_;
        std::shared_ptr<KeyProvider> key_provider_;
        std::shared_ptr<NonceGenerator> nonce_gen_;
        uint8_t internal_buf_[COSE_INTERNAL_BUF_SIZE]{};
    };

}

#endif
