// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace klr::compiler;

TEST_CASE("For numerical loop")
{
    Lexer lexer("for (var i = 0; i < 10; i += 1) {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::FOR,
          TokenType::LEFT_PAREN,
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::IDENTIFIER,
          TokenType::LESS,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::IDENTIFIER,
          TokenType::PLUS_EQ,
          TokenType::NUM_LITERAL,
          TokenType::RIGHT_PAREN,
          TokenType::LEFT_BRACE,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("For range loop")
{
    Lexer lexer("for item in items {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::FOR,
          TokenType::IDENTIFIER,
          TokenType::IN,
          TokenType::IDENTIFIER,
          TokenType::LEFT_BRACE,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("For range tuple loop")
{
    Lexer lexer("for [i1, i2] in items {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::FOR,
          TokenType::LEFT_BRACKET,
          TokenType::IDENTIFIER,
          TokenType::COMMA,
          TokenType::IDENTIFIER,
          TokenType::RIGHT_BRACKET,
          TokenType::IN,
          TokenType::IDENTIFIER,
          TokenType::LEFT_BRACE,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("While loop")
{
    Lexer lexer("while (x > 0) { x -= 1; }");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::WHILE,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER,
          TokenType::GREATER,
          TokenType::NUM_LITERAL,
          TokenType::RIGHT_PAREN,
          TokenType::LEFT_BRACE,
          TokenType::IDENTIFIER,
          TokenType::MINUS_EQ,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Switch cases")
{
    Lexer lexer(R"(
            switch (x)
            {
                case 1:
                    return 1;
                case 2:
                    return 2;
                default:
                    return 0;
            }
        )");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::SWITCH,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER,
          TokenType::RIGHT_PAREN,
          TokenType::LEFT_BRACE,
          TokenType::CASE,
          TokenType::NUM_LITERAL,
          TokenType::COLON,
          TokenType::RETURN,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::CASE,
          TokenType::NUM_LITERAL,
          TokenType::COLON,
          TokenType::RETURN,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::DEFAULT,
          TokenType::COLON,
          TokenType::RETURN,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("If statement")
{
    Lexer lexer(R"(
            var x: i32 = 1;
            var y: i32 = 2;
            if (x > y)
            {
                x = y;
            }
        )");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER, // x
          TokenType::COLON,
          TokenType::I32,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::VAR,
          TokenType::IDENTIFIER, // y
          TokenType::COLON,
          TokenType::I32,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::IF,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER,
          TokenType::GREATER,
          TokenType::IDENTIFIER,
          TokenType::RIGHT_PAREN,
          TokenType::LEFT_BRACE,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}
