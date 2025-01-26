// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Multi-character operators")
{
    std::vector<std::pair<std::string, TokenType> > operators = {
        { "->", TokenType::ARROW },
        { "::", TokenType::SCOPE },
        { "..", TokenType::RANGE },
        { "...", TokenType::SPREAD },
        { "&&", TokenType::LOGICAL_AND },
        { "||", TokenType::LOGICAL_OR },
        { ">=", TokenType::GE },
        { "<=", TokenType::LE },
        { "==", TokenType::EQ },
        { "!=", TokenType::NE },
        { "+=", TokenType::PLUS_EQ },
        { "-=", TokenType::MINUS_EQ },
        { "*=", TokenType::STAR_EQ },
        { "/=", TokenType::SLASH_EQ },
        { "%=", TokenType::PERCENT_EQ },
        { "&=", TokenType::AND_EQ },
        { "|=", TokenType::OR_EQ },
        { "^=", TokenType::XOR_EQ },
        { "<<", TokenType::LEFT_SHIFT },
        { ">>", TokenType::RIGHT_SHIFT },
        { "<<=", TokenType::LEFT_SHIFT_EQ },
        { ">>=", TokenType::RIGHT_SHIFT_EQ }
    };

    for (const auto &[op, type]: operators)
    {
        Lexer lexer(op);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}