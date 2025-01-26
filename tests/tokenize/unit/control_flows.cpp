// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Control flow")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "if", TokenType::IF },
        { "else", TokenType::ELSE },
        { "for", TokenType::FOR },
        { "while", TokenType::WHILE },
        { "break", TokenType::BREAK },
        { "continue", TokenType::CONTINUE },
        { "switch", TokenType::SWITCH },
        { "case", TokenType::CASE },
        { "default", TokenType::DEFAULT }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}