set(CBOR_LIB "NanoCBOR" CACHE STRING "Cbor library.") # libmcu_cbor, NanoCBOR
set(CRYPTO_LIB "ascon-c" CACHE STRING "Crypto library.") # ascon-c, mbedtls
set(KDF_ALGO "HKDF-Ascon256" CACHE STRING "Algorithm for key derivation.")
set(PIV_LEN 2 CACHE STRING "Length of Partial IV.") # Max 16
