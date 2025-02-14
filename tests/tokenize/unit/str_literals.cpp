// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace klr::compiler;

TEST_CASE("String literals")
{
    SECTION("Basic strings")
    {
        std::vector<std::string> strings = {
            "\"Hello, world!\"",
            "\"\"",  // empty
            "\"123\"",
            "\"Special chars: !@#$%^&*()\""
        };

        for (const auto& str : strings)
        {
            Lexer lexer(str);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::STR_LITERAL);
            CHECK(tokens->lens[0] == str.length());
        }
    }

    SECTION("Strings with escapes")
    {
        std::vector<std::string> strings = {
            R"("Escaped quotes: \"")",
            R"("Newline: \n")",
            R"("Tab: \t")",
            R"("Unicode: \u1234")"
        };

        for (const auto& str : strings)
        {
            Lexer lexer(str);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::STR_LITERAL);
        }
    }
}