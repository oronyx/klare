// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace klr::compiler;

TEST_CASE("Identifier")
{
    SECTION("Valid")
    {
        std::vector<std::string> identifiers = {
            "foo",
            "bar123",
            "_private",
            "camelCase",
            "snake_case",
            "SCREAMING_SNAKE",
        };

        for (const auto &id: identifiers)
        {
            Lexer lexer(id);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            // std::cout << token_to_str(tokens->types[0]) << "\n";
            CHECK(tokens->types[0] == TokenType::IDENTIFIER);
            CHECK(tokens->lens[0] == id.length());
        }
    }

    SECTION("Invalid")
    {
        std::vector<std::string> invalid_identifiers = {
            "123abc",
            "@invalid",
            "αβγ",
        };

        for (const auto &id: invalid_identifiers)
        {
            Lexer lexer(id);
            const auto tokens = lexer.tokenize();
            // std::cout << token_to_str(tokens->types[0]) << "\n";
            CHECK(tokens->types[0] == TokenType::UNKNOWN);
        }
    }
}