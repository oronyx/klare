// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Variable")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var x: i32 = 0;");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();
    Lexer lexer(relative_filename, "const x: i32 = 0;");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "const arr: i32[] = { 1, 2, 3, 4, 5 };");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var x: i32 = 1 + 2 * 3 / 4 - 5;");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, R"(
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var res = foo();");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var res = foo.bar();");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var nop = function(x: i32) -> void { return; };");
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
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, "var ;");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::VAR,
          TokenType::SEMICOLON,
          TokenType::END_OF_FILE
          });
}
