#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

#include <cstdint>

namespace rmw_desert::security {
    enum AeadAlgorithms {
        ASCON_AEAD128,
        ASCON_AEAD128_64,
        ASCON_AEAD128_32,
        A128GCM
    };

    class AeadParams {
    public:
        AeadParams(AeadAlgorithms alg, uint16_t key_size, uint16_t nonce_size, uint16_t tag_size)
            : alg_(alg), key_size_(key_size), nonce_size_(nonce_size), tag_size_(tag_size) {}

        [[nodiscard]] AeadAlgorithms get_alg() const { return alg_; }
        [[nodiscard]] uint16_t get_key_size() const { return key_size_; }
        [[nodiscard]] uint16_t get_nonce_size() const { return nonce_size_; }
        [[nodiscard]] uint16_t get_tag_size() const { return tag_size_; }

    private:
        AeadAlgorithms alg_;
        uint16_t key_size_;
        uint16_t nonce_size_;
        uint16_t tag_size_;
    };
}

#endif
