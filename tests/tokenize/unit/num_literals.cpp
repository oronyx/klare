// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace klr::compiler;

TEST_CASE("Number literals")
{
    SECTION("Integer literals")
    {
        std::vector<std::string> integers = {
            "0",
            "123",
            "0xFF",
            "0b1010",
        };

        for (const auto &num: integers)
        {
            Lexer lexer(num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }

    SECTION("Float literals")
    {
        std::vector<std::string> floats = {
            "0.0",
            "123.456",
            "1e10",
            "1.23e-4",
        };

        for (const auto &num: floats)
        {
            Lexer lexer(num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }
}