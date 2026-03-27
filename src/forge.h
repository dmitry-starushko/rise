#pragma once
#ifndef FORGE_H
#define FORGE_H

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
#include <array>
#include <cstddef>
#include <expected>
#include <flat_map>
#include <format>
#include <functional>
#include <memory>

namespace fg {

constexpr bool check_duplicates = true;
constexpr size_t max_kind_size = 64;
using artifact_kind_t = std::array<char, max_kind_size>;

namespace literals {

consteval auto operator""_art_kind (const char *str, size_t size) {
    artifact_kind_t iname {};
    iname.fill(0);
    std::copy(str, str + std::min(size, iname.size() - 1), iname.data());
    return iname;
}

}

template<typename> class forge;
template<typename, artifact_kind_t, typename, auto ...> class artifact;

template<typename Base>
class forge {
  public:

    forge() = delete;

    INLINE
    static std::expected<std::unique_ptr<Base>, std::nullptr_t> craft(const std::string_view art_kind) noexcept {
        if (auto spawner = spawners().find(art_kind); spawner != spawners().end()) {
            return spawner->second();
        }

        return std::unexpected(nullptr);
    }

    INLINE
    static std::vector<std::string_view> recipes() {
        return spawners().keys();
    }

  private:

    using spawner_t = std::function<std::unique_ptr<Base>()>;

    INLINE
    static bool store_recipe(std::string_view art_kind, spawner_t &&spawner) noexcept(!check_duplicates) {
        if constexpr (check_duplicates) {
            if (spawners().contains(art_kind)) {
                throw std::runtime_error(std::format("Recipe for '{}' already stored", art_kind));
            }
        }

        spawners().emplace(art_kind, std::move(spawner));
        return true;
    }

    INLINE
    static auto &spawners() noexcept {
        static std::flat_map<std::string_view, spawner_t> _spawners{};
        return _spawners;
    }

    template<typename, artifact_kind_t, typename, auto ...> friend class artifact;
};

template<typename T, artifact_kind_t art_kind, typename Base, auto ... args>
class artifact {
  protected:

    INLINE
    static std::string_view artifact_kind() {
        static const artifact_kind_t kind {std::move(art_kind)};
        return std::string_view(kind.data());
    }

    INLINE
    static bool __ensure__recipe__stored__() noexcept {
        return _recipe_stored;
    }

  private:

    INLINE
    static bool store_recipe() noexcept(noexcept(forge<Base>::store_recipe(artifact_kind(), [] {return nullptr;}))) {
        return forge<Base>::store_recipe(artifact_kind(), [] {
            return std::make_unique<T>(args...);
        });
    }

    inline static const bool _recipe_stored = store_recipe();
};

}

#define ARTIFACT inline static void __artifact__activator__() noexcept { __ensure__recipe__stored__(); }

#endif // FORGE_H
