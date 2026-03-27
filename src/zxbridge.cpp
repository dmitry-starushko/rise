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

#include "zxbridge.h"

/*INDENT-OFF*/

namespace zk = zx::kbd;

ZxBridge::ZxBridge() noexcept:
    _key_map {
    {KSC::K1,           {zk::key::D1}},
    {KSC::K2,           {zk::key::D2}},
    {KSC::K3,           {zk::key::D3}},
    {KSC::K4,           {zk::key::D4}},
    {KSC::K5,           {zk::key::D5}},
    {KSC::K6,           {zk::key::D6}},
    {KSC::K7,           {zk::key::D7}},
    {KSC::K8,           {zk::key::D8}},
    {KSC::K9,           {zk::key::D9}},
    {KSC::K0,           {zk::key::D0}},
    {KSC::Q,            {zk::key::Q}},
    {KSC::W,            {zk::key::W}},
    {KSC::E,            {zk::key::E}},
    {KSC::R,            {zk::key::R}},
    {KSC::T,            {zk::key::T}},
    {KSC::Y,            {zk::key::Y}},
    {KSC::U,            {zk::key::U}},
    {KSC::I,            {zk::key::I}},
    {KSC::O,            {zk::key::O}},
    {KSC::P,            {zk::key::P}},
    {KSC::A,            {zk::key::A}},
    {KSC::S,            {zk::key::S}},
    {KSC::D,            {zk::key::D}},
    {KSC::F,            {zk::key::F}},
    {KSC::G,            {zk::key::G}},
    {KSC::H,            {zk::key::H}},
    {KSC::J,            {zk::key::J}},
    {KSC::K,            {zk::key::K}},
    {KSC::L,            {zk::key::L}},
    {KSC::Z,            {zk::key::Z}},
    {KSC::X,            {zk::key::X}},
    {KSC::C,            {zk::key::C}},
    {KSC::V,            {zk::key::V}},
    {KSC::B,            {zk::key::B}},
    {KSC::N,            {zk::key::N}},
    {KSC::M,            {zk::key::M}},
    {KSC::LEFT_SHIFT,   {zk::key::CAPS_SHIFT}},
    {KSC::RIGHT_SHIFT,  {zk::key::CAPS_SHIFT}},
    {KSC::LEFT_CTRL,    {zk::key::SYMBOL_SHIFT}},
    {KSC::RIGHT_CTRL,   {zk::key::SYMBOL_SHIFT}},
    {KSC::SPACE,        {zk::key::SPACE}},
    {KSC::ENTER,        {zk::key::ENTER}},
    {KSC::MINUS,        {zk::key::SYMBOL_SHIFT, zk::key::J}},
    {KSC::EQUAL,        {zk::key::SYMBOL_SHIFT, zk::key::L}},
    {KSC::BACK,         {zk::key::CAPS_SHIFT, zk::key::D0}},
    {KSC::SEMICOLON,    {zk::key::SYMBOL_SHIFT, zk::key::O}},
    {KSC::APOSTROPHE,   {zk::key::SYMBOL_SHIFT, zk::key::D7}},
    {KSC::COMMA,        {zk::key::SYMBOL_SHIFT, zk::key::N}},
    {KSC::DOT,          {zk::key::SYMBOL_SHIFT, zk::key::M}},
    {KSC::SLASH,        {zk::key::SYMBOL_SHIFT, zk::key::C}},
    {KSC::RIGHT_ALT,    {zk::key::SYMBOL_SHIFT, zk::key::CAPS_SHIFT}},
    {KSC::NUM_SLASH,    {zk::key::SYMBOL_SHIFT, zk::key::V}},
    {KSC::NUM_ASTERISK, {zk::key::SYMBOL_SHIFT, zk::key::B}},
    {KSC::NUM_MINUS,    {zk::key::SYMBOL_SHIFT, zk::key::J}},
    {KSC::NUM_PLUS,     {zk::key::SYMBOL_SHIFT, zk::key::K}},
    {KSC::NUM_ENTER,    {zk::key::ENTER}},
    {KSC::NUM_DOT,      {zk::key::SYMBOL_SHIFT, zk::key::M}},
    {KSC::NUM_1,        {zk::key::D1}},
    {KSC::NUM_2,        {zk::key::D2}},
    {KSC::NUM_3,        {zk::key::D3}},
    {KSC::NUM_4,        {zk::key::D4}},
    {KSC::NUM_5,        {zk::key::D5}},
    {KSC::NUM_6,        {zk::key::D6}},
    {KSC::NUM_7,        {zk::key::D7}},
    {KSC::NUM_8,        {zk::key::D8}},
    {KSC::NUM_9,        {zk::key::D9}},
    {KSC::NUM_0,        {zk::key::D0}},
} {
}

/*INDENT-ON*/
