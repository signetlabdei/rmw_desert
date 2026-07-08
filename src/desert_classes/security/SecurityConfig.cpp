#include <memory>

#include "SecurityConfig.h"
#include "SecurityLayer.h"

namespace rmw_desert::security {

    std::shared_ptr<AeadParams> ascon_aead128 = std::make_shared<AeadParams>(ASCON_AEAD128, 16, 16, 16);
    std::shared_ptr<AeadParams> ascon_aead128_64 = std::make_shared<AeadParams>(ASCON_AEAD128_64, 16, 16, 8);
    std::shared_ptr<AeadParams> ascon_aead128_32 = std::make_shared<AeadParams>(ASCON_AEAD128_32, 16, 16, 4);
    std::shared_ptr<AeadParams> aes_gcm_128 = std::make_shared<AeadParams>(A128GCM, 16, 12, 16);

    std::shared_ptr<KeyProvider> key_provider = std::make_shared<EnvKeyProvider>("MASTER_SECRET_KEY");
    std::shared_ptr<NonceGenerator> nonce_gen = std::make_shared<DefaultNVMNonceGenerator>();

    const std::unique_ptr<SecurityLayer> g_sec_layer = std::make_unique<CoseSecurityLayer>(
        aes_gcm_128, key_provider, nonce_gen
    );

}

