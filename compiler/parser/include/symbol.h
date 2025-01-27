// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#pragma once

#include <cstdint>

namespace orx::compiler
{
    struct Symbol
    {
        enum class Type : uint8_t
        {
            VAR,
            CONST,
            FUNCTION,
            MODULE
        } type;
        uint32_t name_index;
        uint32_t type_idx;      // index into type table for infer
        uint16_t scope_level;
        uint16_t flags; // used, Initialized, etc
    };
}
