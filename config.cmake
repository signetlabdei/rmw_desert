set(CBOR_LIB "NanoCBOR" CACHE STRING "Cbor library.") # libmcu_cbor, NanoCBOR
option(SECURE_MODE_ENABLED "Enable COSE Security Layer." ON)
option(COSE_STATELESS_COMP_ENABLED "Enable COSE stateless compression." ON)
set(CRYPTO_LIB "ascon-c" CACHE STRING "Crypto library.") # ascon-c, mbedtls
set(AEAD_ALGO "Ascon-AEAD128" CACHE STRING "Algorithm for authenticated encryption with additional data.") # Ascon-AEAD128, Ascon-AEAD128-64, Ascon-AEAD128-32
set(KDF_ALGO "HKDF-Ascon256" CACHE STRING "Algorithm for key derivation.")
set(PIV_LEN 2 CACHE STRING "Length of Partial IV.") # Max 16
set(SENDER_ID "0x01" CACHE STRING "Sender ID in hex.")
set(RECEIVER_ID "0x02" CACHE STRING "Receiver ID in hex.")
