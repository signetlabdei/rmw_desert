set(CBOR_LIB "NanoCBOR" CACHE STRING "Cbor library.") # libmcu_cbor, NanoCBOR
option(SECURE_MODE_ENABLED "" ON)
set(CRYPTO_LIB "mbedtls" CACHE STRING "Crypto library.")
set(AEAD_ALGO "Ascon-AEAD128" CACHE STRING "Algorithm for authenticated encryption with additional data.")
set(KDF_ALGO "HKDF-Ascon256" CACHE STRING "Algorithm for key derivation.")
set(SENDER_ID "0x01" CACHE STRING "Sender ID in hex.")
set(RECEIVER_ID "0x02" CACHE STRING "Receiver ID in hex.")
