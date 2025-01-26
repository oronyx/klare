// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Comments")
{
    SECTION("Single")
    {
        Lexer lexer(R"(// This is a single-line comment)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }

    SECTION("Multi")
    {
        Lexer lexer(R"(/*
        This is a multi-line comment
        TEST
        TEST
        TEST
        TEST
        TEST
        */
)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }
}