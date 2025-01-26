// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("EOF")
{
    Lexer lexer("");
    const auto tks = lexer.tokenize();
    REQUIRE(tks->size() == 1);
}