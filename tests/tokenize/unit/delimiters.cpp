// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Delimiters")
{
    std::vector<std::pair<std::string, TokenType> > delimiters = {
        { "(", TokenType::LEFT_PAREN },
        { ")", TokenType::RIGHT_PAREN },
        { "{", TokenType::LEFT_BRACE },
        { "}", TokenType::RIGHT_BRACE },
        { "[", TokenType::LEFT_BRACKET },
        { "]", TokenType::RIGHT_BRACKET },
        { ",", TokenType::COMMA },
        { ":", TokenType::COLON },
        { ";", TokenType::SEMICOLON },
        { "?", TokenType::QUESTION }
    };

    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();
    for (const auto &[delimiter, type]: delimiters)
    {
        Lexer lexer(relative_filename, delimiter);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}