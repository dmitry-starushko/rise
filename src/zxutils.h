#pragma once
#ifndef ZXUTILS_H
#define ZXUTILS_H

/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *  ____  ___ ____  _____
 * |  _ \|_ _/ ___|| ____|
 * | |_) || |\___ \|  _|
 * |  _ < | | ___) | |___
 * |_| \_\___|____/|_____|
 *
 * Copyright (C) 2026 Dmitry Starushko <dmitry.starushko@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 */

#include "inline.h"
#include <expected>
#include <string>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <fstream>
#include <format>

namespace zxutils {

template<long max_file_size = 0x1000000>
INLINE
inline std::expected<std::vector<std::uint8_t>, std::string> read_file(const std::filesystem::path &path) noexcept {
    using char_ptr = std::ifstream::char_type*;

    if (std::ifstream file(path, std::ios::binary | std::ios::ate); file) {
        if (auto size = file.tellg(); 0 < size && size <= max_file_size) {
            if (std::vector<uint8_t> bytes(size); file.seekg(0) && file.read(reinterpret_cast<char_ptr>(bytes.data()), bytes.size())) {
                return bytes;
            } else {
                return std::unexpected(std::format("File read error: {}", path.generic_string()));
            }
        } else {
            return std::unexpected(std::format("Wrong file size: {} -- {} bytes", path.generic_string(), static_cast<int>(size)));
        }
    } else {
        return std::unexpected(std::format("Cannot open file: {}", path.generic_string()));
    }
}

INLINE
inline std::expected<bool, std::string> save_file(const std::filesystem::path &path, const std::span<uint8_t> data) noexcept {
    using char_ptr = std::ofstream::char_type *;

    if (std::ofstream file(path, std::ios::binary); file) {
        if (file.write(reinterpret_cast<char_ptr>(data.data()), data.size())) {
            return true;
        } else {
            return std::unexpected(std::format("File write error: {}", path.generic_string()));
        }
    } else {
        return std::unexpected(std::format("Cannot open file: {}", path.generic_string()));
    }
}

template<typename T>
INLINE
inline constexpr const T &consume_as(const auto &bytes, auto &i) {
    if (auto j = i + sizeof(T); j > bytes.end()) [[unlikely]] {
        throw std::out_of_range("Unexpected end of data");
    } else {
        const uint8_t *ptr = &*i;
        const T& result = *reinterpret_cast<const T *>(ptr);
        return i = j, result;
    }
};

};

#endif // ZXUTILS_H
