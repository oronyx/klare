// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Single-character operators")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    std::vector<std::pair<std::string, TokenType> > operators = {
        { "+", TokenType::PLUS },
        { "-", TokenType::MINUS },
        { "*", TokenType::STAR },
        { "/", TokenType::SLASH },
        { "%", TokenType::PERCENT },
        { "=", TokenType::EQUAL },
        { "!", TokenType::BANG },
        { "<", TokenType::LESS },
        { ">", TokenType::GREATER },
        { "&", TokenType::AND },
        { "|", TokenType::OR },
        { "^", TokenType::XOR },
        { "~", TokenType::TILDE },
        { ".", TokenType::DOT }
    };

    for (const auto &[op, type]: operators)
    {
        Lexer lexer(relative_filename, op);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}