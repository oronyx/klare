// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Keyword")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "true", TokenType::TRUE },
        { "false", TokenType::FALSE },
        { "null", TokenType::NIL },
        { "import", TokenType::IMPORT },
        { "var", TokenType::VAR },
        { "const", TokenType::CONST },
        { "function", TokenType::FUNCTION },
        { "inline", TokenType::INLINE },
        { "return", TokenType::RETURN },
        { "enum", TokenType::ENUM }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
        CHECK(tokens->lens[0] == keyword.length());
    }
}