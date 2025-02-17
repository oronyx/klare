// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("No params")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "function main() -> void {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "function add(x: i32) -> i32 {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "function add(x: i32, y: i32) -> i32 {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

   Lexer lexer(relative_filename, "function x<T>(a: T) -> T {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

   Lexer lexer(relative_filename, "function x<T, U>(a: T, b: U) -> U {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

   Lexer lexer(relative_filename, "function v<T...>(...: T...) -> void {}");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "function x<T, U...>(a: T, ...args: U...) -> T {}");
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

