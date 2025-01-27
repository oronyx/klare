// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Variable")
{
    Lexer lexer("var x: i32 = 0;");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::I32,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Const")
{
    Lexer lexer("const x: i32 = 0;");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::CONST,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::I32,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Array")
{
    Lexer lexer("const arr: i32[] = { 1, 2, 3, 4, 5 };");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::CONST,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::I32,
          TokenType::LEFT_BRACKET,
          TokenType::RIGHT_BRACKET,
          TokenType::EQUAL,
          TokenType::LEFT_BRACE,
          TokenType::NUM_LITERAL,
          TokenType::COMMA,
          TokenType::NUM_LITERAL,
          TokenType::COMMA,
          TokenType::NUM_LITERAL,
          TokenType::COMMA,
          TokenType::NUM_LITERAL,
          TokenType::COMMA,
          TokenType::NUM_LITERAL,
          TokenType::RIGHT_BRACE,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Binary expression")
{
    Lexer lexer("var x: i32 = 1 + 2 * 3 / 4 - 5;");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::I32,
          TokenType::EQUAL,
          TokenType::NUM_LITERAL,
          TokenType::PLUS,
          TokenType::NUM_LITERAL,
          TokenType::STAR,
          TokenType::NUM_LITERAL,
          TokenType::SLASH,
          TokenType::NUM_LITERAL,
          TokenType::MINUS,
          TokenType::NUM_LITERAL,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Statement")
{
    Lexer lexer(R"(
            var x: i32 = 0;
            var y: i32 = 2;
            var z = x > y ? x : y; // Short hand
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
          TokenType::VAR,
          TokenType::IDENTIFIER, // z
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::GREATER,
          TokenType::IDENTIFIER,
          TokenType::QUESTION,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Function call")
{
    Lexer lexer("var res = foo();");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER, // res
          TokenType::EQUAL,
          TokenType::IDENTIFIER, // foo
          TokenType::LEFT_PAREN,
          TokenType::RIGHT_PAREN,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Method invoke")
{
    Lexer lexer("var res = foo.bar();");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER, // res
          TokenType::EQUAL,
          TokenType::IDENTIFIER, // foo
          TokenType::DOT,
          TokenType::IDENTIFIER, // bar
          TokenType::LEFT_PAREN,
          TokenType::RIGHT_PAREN,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Lambda")
{
    Lexer lexer("var nop = function(x: i32) -> void { return; };");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::IDENTIFIER, // nop
          TokenType::EQUAL,
          TokenType::FUNCTION,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER, // x
          TokenType::COLON,
          TokenType::I32,
          TokenType::RIGHT_PAREN,
          TokenType::ARROW,
          TokenType::VOID,
          TokenType::LEFT_BRACE,
          TokenType::RETURN,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Invalid declaration")
{
    Lexer lexer("var ;");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}
