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
 * @file sec_utils.h
 * @brief Utilities for the security layer
 * 
 * This header declares utility functions shared by the security layer.
 * It includes helpers for converting hexadecimal strings into byte vectors
 * and small generic macros used while processing cryptographic input data.
 * These utilities support configuration parsing and internal data handling.
 *
 * @author Dmytro Ochkas
 *
 */

#ifndef SEC_UTILS_H
#define SEC_UTILS_H

/** @cond */

#include <string>
#include <vector>
#include <cstdint>

/** @endcond */

#define MIN(a, b) ((a) > (b) ? (b) : (a))

/**
 * @brief Decodes a hexadecimal string into a byte vector.
 *
 * The input can optionally begin with the prefixes @c 0x or @c 0X.
 * After an optional prefix, the string must contain an even number of
 * valid hexadecimal characters.
 *
 * @param hex String containing hexadecimal byte values.
 * @param out Output vector receiving the decoded bytes.
 * @return true if the complete input is valid and has been decoded;
 *         false if the input has an odd length or contains non-hexadecimal
 *         characters.
 *
 * @note On failure, @p out is cleared.
 */
bool decode_hex(const std::string& hex, std::vector<uint8_t>& out);

#endif
