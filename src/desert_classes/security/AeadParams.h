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
 * @file AeadParams.h
 * @brief Class containing the parameters of the selected cryptography
 * 
 * This header defines the algorithms and parameter sets used by the security 
 * layer. It provides AEAD and KDF identifiers together with the key, nonce, 
 * and authentication-tag sizes required by each supported AEAD algorithm. 
 * These parameters are used to configure cryptographic context derivation.
 *
 * @author Dmytro Ochkas
 *
 */

#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

/** @cond */

#include <cstdint>

/** @endcond */

#define AEAD_ALGO_ASCON_128 "Ascon-AEAD128"
#define AEAD_ALGO_ASCON_128_64 "Ascon-AEAD128-64"
#define AEAD_ALGO_ASCON_128_32 "Ascon-AEAD128-32"

#define KDF_ALGO_ASCON_256 "HKDF-Ascon256"

namespace security
{

enum AeadAlgorithms
{
  ASCON_AEAD128,
  ASCON_AEAD128_64,
  ASCON_AEAD128_32,
  A128GCM,
  AEAD_UNKNOWN
};

enum KdfAlgorithms
{
  HKDF_ASCON,
  HKDF_HMAC256,
  KDF_UNKNOWN
};

class AeadParams
{
  public:
   /**
    * @brief Creates the parameter set for an AEAD algorithm.
    *
    * @param alg AEAD algorithm to configure.
    */
    AeadParams(AeadAlgorithms alg);

  /**
    * @brief Returns the configured AEAD algorithm.
    *
    * @return The selected AEAD algorithm identifier.
    */
    AeadAlgorithms get_alg() const { return alg_; }

   /**
    * @brief Returns the size of the AEAD key.
    *
    * @return Key size in bytes.
    */
    uint16_t get_key_size() const { return key_size_; }

   /**
    * @brief Returns the size of the AEAD nonce.
    *
    * @return Nonce size in bytes.
    */
    uint16_t get_nonce_size() const { return nonce_size_; }

   /**
    * @brief Returns the size of the AEAD authentication tag.
    *
    * @return Authentication-tag size in bytes.
    */
    uint16_t get_tag_size() const { return tag_size_; }

  private:
    AeadAlgorithms alg_;
    uint16_t key_size_;
    uint16_t nonce_size_;
    uint16_t tag_size_;
};

}  // namespace security

#endif
