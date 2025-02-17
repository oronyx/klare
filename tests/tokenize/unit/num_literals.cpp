// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Number literals")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    SECTION("Integer literals")
    {
        std::vector<std::string> integers = {
            "0",
            "123",
            "0xFF",
            "0b1010",
        };

        for (const auto &num: integers)
        {
            Lexer lexer(relative_filename, num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }

    SECTION("Float literals")
    {
        std::vector<std::string> floats = {
            "0.0",
            "123.456",
            "1e10",
            "1.23e-4",
        };

        for (const auto &num: floats)
        {
            Lexer lexer(relative_filename, num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }
}