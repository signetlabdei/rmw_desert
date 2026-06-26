/****************************************************************************
 * Copyright (C) 2026 Davide Costa                                          *
 *                                                                          *
 * This file is part of RMW desert.                                         *
 *                                                                          *
 *   RMW desert is free software: you can redistribute it and/or modify it  *
 *   under the terms of the GNU General Public License as published by the  *
 *   Free Software Foundation, either version 3 of the License, or any      *
 *   later version.                                                         *
 *                                                                          *
 *   RMW desert is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with RMW desert.  If not, see <http://www.gnu.org/licenses/>.    *
 ****************************************************************************/

/**
 * @file SecurityLayer.h
 * @brief Class used to handle the encryption of the stream
 * 
 * This header declares the security layer used to protect application data.
 * It derives sender and receiver contexts, builds nonces, and performs COSE
 * wrapping and unwrapping of payloads using the selected AEAD and KDF schemes.
 * It also manages key material, identifiers, and serialization buffers.
 *
 * @author Dmytro Ochkas
 *
 */

#ifndef SECURITY_LAYER_H
#define SECURITY_LAYER_H

/** @cond */

#include <memory>
#include <string>
#include <vector>

/** @endcond */

#include "rmw/error_handling.h"

#include <cose_defines.h>
#include <cose/key.h>

#include "AeadParams.h"

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
   /**
    * @brief Creates a security layer using compile-time configuration values.
    *
    * @throws std::runtime_error If identifiers or key material are invalid,
    *                            or if the security context cannot be derived.
    */
    SecurityLayer();

   /**
    * @brief Creates a security layer from explicitly supplied parameters.
    *
    * @param aead_params Parameters of the AEAD algorithm to use.
    * @param kdf Key-derivation algorithm to use.
    * @param master_key Master secret used to derive encryption keys.
    * @param master_salt Salt used during key derivation.
    * @param piv_size Size, in bytes, of the Partial IV carried in messages.
    * @param sender_seq_number Initial sender sequence number.
    * @param sender_id Sender identifier used to derive the sender key.
    * @param receiver_id Receiver identifier used to derive the receiver key.
    * @throws std::runtime_error If the security context cannot be derived.
    */
    SecurityLayer(const AeadParams &aead_params, KdfAlgorithms kdf, const std::vector<uint8_t> &master_key,
      const std::vector<uint8_t> &master_salt, size_t piv_size, size_t sender_seq_number,
      const std::vector<uint8_t> &sender_id, const std::vector<uint8_t> &receiver_id);

   /**
    * @brief Encrypts and serializes an application payload as a COSE message.
    *
    * Generates a Partial IV from the current sender sequence number, adds it 
    * as an unprotected COSE header, encrypts the payload with the sender key, 
    * and optionally applies the stateless COSE compression format.
    *
    * On success, the sender sequence number is incremented.
    *
    * @param data Buffer containing the plaintext payload and receiving output.
    * @param data_len Length of the plaintext payload in bytes.
    * @param data_max_len Total capacity of @p data in bytes.
    * @param cose_ptr Output pointer to the beginning of the encoded message.
    * @param cose_len Output length of the encoded COSE message in bytes.
    * @return OK on success; otherwise an error describing the failed step.
    *
    * @note The caller must ensure that @p data has sufficient capacity for
    *       the resulting serialized message.
    */
    SecurityResult wrap(uint8_t* data, size_t data_len, size_t data_max_len, uint8_t** cose_ptr, size_t* cose_len);

   /**
    * @brief Decodes, authenticates, and decrypts a COSE message.
    *
    * If stateless compression is enabled, the method first reconstructs the
    * standard COSE serialization. It then decrypts the message using the
    * receiver key and copies the recovered plaintext to @p data.
    *
    * @param data Buffer containing the received COSE message. On success,
    *             it is overwritten with the plaintext payload.
    * @param data_len Length of the received message in bytes.
    * @param data_max_len Total capacity of @p data in bytes.
    * @param new_data_len Output length of the recovered plaintext in bytes.
    * @return OK on success; UNWRAP_ERROR if parsing, authentication, or
    *         decryption fails; otherwise a specific processing error.
    */
    SecurityResult unwrap(uint8_t* data, size_t data_len, size_t data_max_len, size_t* new_data_len);

  private:
    SecurityResult get_master_key_env(std::vector<uint8_t>& master_key) const;
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
