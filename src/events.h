#pragma once
#ifndef EVENTS_H
#define EVENTS_H

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
#include <functional>
#include <flat_map>
#include <utility>

enum class EVENT;

auto consteval operator, (const EVENT ev, auto variant) noexcept {
    return std::make_pair(ev, variant);
}

namespace event {

template<EVENT> struct data;
template<EVENT> class default_bus;
template<EVENT> class fast_unsafe_bus;
template<EVENT> class null_bus;
template<typename T, EVENT ... event> class handler;
template<EVENT, auto variant> class default_variant_bus;
template<EVENT, auto variant> class fast_unsafe_variant_bus;
template<EVENT, auto variant> class null_variant_bus;
template<typename T, auto ... event_variant> class variant_handler;

template<typename T, EVENT event> concept able_to_handle
= requires(T &t, data<event> &d) {
    {
        t.handle(d)
    };
};

template<typename T, EVENT event, auto variant> concept able_to_handle_variant
= requires(T &t, data<event> &d) {
    {
        t.template handle<variant>(d)
    };
};

//--------------------------------------------------------------------------------

template<EVENT event>
class registry {
  public:

    using _handler_t = std::function<void(data<event>&)>;

    registry() = delete;

  private:

    INLINE
    static void connect(void *ptr, _handler_t &&f) noexcept {
        _handlers.emplace(ptr, std::move(f));
    }

    INLINE
    static void disconnect(void *ptr) noexcept {
        _handlers.erase(ptr);
    }

  protected:

    inline static std::flat_map<void *, _handler_t> _handlers{};
    template<typename T, EVENT... e> friend class handler;
};

template<EVENT event>
class default_bus: public registry<event> {
  public:

    using inherited = registry<event>;
    using _handler_t = registry<event>::_handler_t;

    default_bus() = delete;

    INLINE
    static void dispatch(data<event> &info) noexcept {
        std::vector<void *> hs(inherited::_handlers.keys());

        for (auto h : hs) {
            if (auto itr = inherited::_handlers.find(h); itr != inherited::_handlers.end()) {
                itr->second(info);
            }
        }
    }
};

template<EVENT event>
class fast_unsafe_bus: public registry<event> {
  public:

    using inherited = registry<event>;
    using _handler_t = registry<event>::_handler_t;

    fast_unsafe_bus() = delete;

    INLINE
    static void dispatch(data<event> &info) noexcept {
        for (const auto &h : inherited::_handlers.values()) {
            h(info);
        }
    }
};

template<EVENT event>
class null_bus: public registry<event> {
  public:

    using inherited = registry<event>;
    using _handler_t = registry<event>::_handler_t;

    null_bus() = delete;

    INLINE
    static void dispatch(data<event> &) noexcept {
    }
};

template<typename T, EVENT ... event>
class handler {
    template<EVENT e>
    INLINE
    void bus_connect() noexcept {
        T *self = static_cast<T *>(this);
        default_bus<e>::connect(self, [self] INLINE (data<e> &ed) noexcept(noexcept(self->handle)) {
            self->handle(ed);
        });
    }

    template<EVENT e>
    INLINE
    void bus_disconnect() noexcept {
        default_bus<e>::disconnect(static_cast<T *>(this));
    }

  protected:

    INLINE handler() noexcept {
        static_assert((able_to_handle<T, event> && ...));
        (bus_connect<event>(), ...);
    }

    INLINE ~handler() noexcept {
        (bus_disconnect<event>(), ...);
    }
};

//--------------------------------------------------------------------------------

template<EVENT event, auto variant>
class variant_registry {
  public:

    using _handler_t = std::function<void(data<event>&)>;

    variant_registry() = delete;

  private:

    INLINE
    static void connect(void *ptr, _handler_t &&f) noexcept {
        _handlers.emplace(ptr, std::move(f));
    }

    INLINE
    static void disconnect(void *ptr) noexcept {
        _handlers.erase(ptr);
    }

  protected:

    inline static std::flat_map<void *, _handler_t> _handlers{};
    template<typename T, auto... e> friend class variant_handler;
};

template<EVENT event, auto variant>
class default_variant_bus: public variant_registry<event, variant> {
  public:

    using inherited = variant_registry<event, variant>;
    using _handler_t = variant_registry<event, variant>::_handler_t;

    default_variant_bus() = delete;

    INLINE
    static void dispatch(data<event> &info) noexcept { // TODO rename ev_data
        std::vector<void *> hs(inherited::_handlers.keys());

        for (auto h : hs) {
            if (auto itr = inherited::_handlers.find(h); itr != inherited::_handlers.end()) {
                itr->second(info);
            }
        }
    }
};

template<EVENT event, auto variant>
class fast_unsafe_variant_bus: public variant_registry<event, variant> {
  public:

    using inherited = variant_registry<event, variant>;
    using _handler_t = variant_registry<event, variant>::_handler_t;

    fast_unsafe_variant_bus() = delete;

    INLINE
    static void dispatch(data<event> &info) noexcept {
        for (const auto &h : inherited::_handlers.values()) {
            h(info);
        }
    }
};

template<EVENT event, auto variant>
class null_variant_bus: public variant_registry<event, variant> {
  public:

    using inherited = variant_registry<event, variant>;
    using _handler_t = variant_registry<event, variant>::_handler_t;

    null_variant_bus() = delete;

    INLINE
    static void dispatch(data<event> &) noexcept {
    }
};

template<typename T, auto ... event_variant>
class variant_handler {
    template<EVENT e, auto v>
    INLINE
    void bus_connect() noexcept {
        T *self = static_cast<T *>(this);
        default_variant_bus<e, v>::connect(self, [self] INLINE (data<e> &ed) noexcept {
            self->template handle<v>(ed);
        });
    }

    template<EVENT e, auto v>
    INLINE
    void bus_disconnect() noexcept {
        default_variant_bus<e, v>::disconnect(static_cast<T *>(this));
    }

  protected:

    INLINE variant_handler() noexcept {
        static_assert((able_to_handle_variant<T, event_variant.first, event_variant.second> && ...));
        (bus_connect<event_variant.first, event_variant.second>(), ...);
    }

    INLINE ~variant_handler() noexcept {
        (bus_disconnect<event_variant.first, event_variant.second>(), ...);
    }
};

template<EVENT event>
INLINE
inline void dispatch(data<event> &e) noexcept {
    default_bus<event>::dispatch(e);
}

template<EVENT event>
INLINE
inline void dispatch(data<event> &&e) noexcept {
    dispatch(e);
}

template<auto variant, EVENT event>
INLINE
inline void dispatch(data<event> &e) noexcept {
    default_variant_bus<event, variant>::dispatch(e);
}

template<auto variant, EVENT event>
INLINE
inline void dispatch(data<event> &&e) noexcept {
    dispatch<variant>(e);
}

#define USE_NULL_BUS(E) template<> class event::default_bus<EVENT::E>: public event::null_bus<EVENT::E> { };
#define USE_FAST_UNSAFE_BUS(E) template<> class event::default_bus<EVENT::E>: public event::fast_unsafe_bus<EVENT::E> { };
#define USE_NULL_VARIANT_BUS(E,V) template<> class event::default_variant_bus<EVENT::E, V>: public event::null_variant_bus<EVENT::E, V> { };
#define USE_FAST_UNSAFE_VARIANT_BUS(E,V) template<> class event::default_variant_bus<EVENT::E, V>: public event::fast_unsafe_variant_bus<EVENT::E, V> { };

}

#endif // EVENTS_H
