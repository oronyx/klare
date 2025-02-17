// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>

using namespace klr::compiler;

TEST_CASE("EOF")
{
    Lexer lexer("");
    const auto tks = lexer.tokenize();
    REQUIRE(tks->size() == 1);
}