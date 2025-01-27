// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("No params")
{
    Lexer lexer("function main() -> void {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
        TokenType::FUNCTION,
        TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN,
        TokenType::RIGHT_PAREN,
        TokenType::ARROW,
        TokenType::VOID,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    });
}

TEST_CASE("Single param")
{
    Lexer lexer("function add(x: i32) -> i32 {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
        TokenType::FUNCTION,
        TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::I32,
        TokenType::RIGHT_PAREN,
        TokenType::ARROW,
        TokenType::I32,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    });
}

TEST_CASE("Multiple params")
{
    Lexer lexer("function add(x: i32, y: i32) -> i32 {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
        TokenType::FUNCTION,
        TokenType::IDENTIFIER,
        TokenType::LEFT_PAREN,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::I32,
        TokenType::COMMA,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::I32,
        TokenType::RIGHT_PAREN,
        TokenType::ARROW,
        TokenType::I32,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    });
}

TEST_CASE("Single generic")
{
   Lexer lexer("function x<T>(a: T) -> T {}");
   const auto tks = lexer.tokenize();
   CHECK(tks->types == std::vector<TokenType>{
       TokenType::FUNCTION,
       TokenType::IDENTIFIER,
       TokenType::LESS,
       TokenType::IDENTIFIER,
       TokenType::GREATER,
       TokenType::LEFT_PAREN,
       TokenType::IDENTIFIER,
       TokenType::COLON,
       TokenType::IDENTIFIER,
       TokenType::RIGHT_PAREN,
       TokenType::ARROW,
       TokenType::IDENTIFIER,
       TokenType::LEFT_BRACE,
       TokenType::RIGHT_BRACE,
       TokenType::END_OF_FILE
   });
}

TEST_CASE("Multiple generics")
{
   Lexer lexer("function x<T, U>(a: T, b: U) -> U {}");
   const auto tks = lexer.tokenize();
   CHECK(tks->types == std::vector<TokenType>{
       TokenType::FUNCTION,
       TokenType::IDENTIFIER,
       TokenType::LESS,
       TokenType::IDENTIFIER,
       TokenType::COMMA,
       TokenType::IDENTIFIER,
       TokenType::GREATER,
       TokenType::LEFT_PAREN,
       TokenType::IDENTIFIER,
       TokenType::COLON,
       TokenType::IDENTIFIER,
       TokenType::COMMA,
       TokenType::IDENTIFIER,
       TokenType::COLON,
       TokenType::IDENTIFIER,
       TokenType::RIGHT_PAREN,
       TokenType::ARROW,
       TokenType::IDENTIFIER,
       TokenType::LEFT_BRACE,
       TokenType::RIGHT_BRACE,
       TokenType::END_OF_FILE
   });
}

TEST_CASE("Variadic")
{
   Lexer lexer("function v<T...>(...: T...) -> void {}");
   const auto tks = lexer.tokenize();
   CHECK(tks->types == std::vector<TokenType>{
       TokenType::FUNCTION,
       TokenType::IDENTIFIER,
       TokenType::LESS,
       TokenType::IDENTIFIER,
       TokenType::SPREAD,
       TokenType::GREATER,
       TokenType::LEFT_PAREN,
       TokenType::SPREAD,
       TokenType::COLON,
       TokenType::IDENTIFIER,
       TokenType::SPREAD,
       TokenType::RIGHT_PAREN,
       TokenType::ARROW,
       TokenType::VOID,
       TokenType::LEFT_BRACE,
       TokenType::RIGHT_BRACE,
       TokenType::END_OF_FILE
   });
}

TEST_CASE("Generic with variadic")
{
    Lexer lexer("function x<T, U...>(a: T, ...args: U...) -> T {}");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
        TokenType::FUNCTION,
        TokenType::IDENTIFIER,
        TokenType::LESS,
        TokenType::IDENTIFIER,
        TokenType::COMMA,
        TokenType::IDENTIFIER,
        TokenType::SPREAD,
        TokenType::GREATER,
        TokenType::LEFT_PAREN,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::IDENTIFIER,
        TokenType::COMMA,
        TokenType::SPREAD,
        TokenType::IDENTIFIER,
        TokenType::COLON,
        TokenType::IDENTIFIER,
        TokenType::SPREAD,
        TokenType::RIGHT_PAREN,
        TokenType::ARROW,
        TokenType::IDENTIFIER,
        TokenType::LEFT_BRACE,
        TokenType::RIGHT_BRACE,
        TokenType::END_OF_FILE
    });
}

