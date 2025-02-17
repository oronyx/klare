// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Multi-character operators")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    std::vector<std::pair<std::string, TokenType>> shift_operators = {
        { "<<", TokenType::LESS },
        { ">>", TokenType::GREATER },
        { "<<=", TokenType::LEFT_SHIFT_EQ },
        { ">>=", TokenType::RIGHT_SHIFT_EQ }
    };

    std::vector<std::pair<std::string, TokenType>> regular_operators = {
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
        { "^=", TokenType::XOR_EQ }
    };

    SECTION("Regular multi-character operators")
    {
        for (const auto &[op, type]: regular_operators)
        {
            Lexer lexer(relative_filename, op);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == type);
        }
    }

    SECTION("Shift operators")
    {
        for (const auto &[op, type]: shift_operators)
        {
            Lexer lexer(relative_filename, op);
            const auto tokens = lexer.tokenize();

            if (op == "<<" || op == ">>")
            {
                REQUIRE(tokens->size() == 3);
                CHECK(tokens->types[0] == type);
                CHECK(tokens->types[1] == type);
                CHECK((static_cast<uint8_t>(tokens->flags[0]) & static_cast<uint8_t>(TokenFlags::COMPOUND_START)) != 0);
                CHECK((static_cast<uint8_t>(tokens->flags[1]) & static_cast<uint8_t>(TokenFlags::COMPOUND_END)) != 0);
            }
            else
            {
                REQUIRE(tokens->size() == 2);
                CHECK(tokens->types[0] == type);
            }
        }
    }
}