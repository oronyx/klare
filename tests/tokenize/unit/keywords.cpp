// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Keyword")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "true", TokenType::TRUE },
        { "false", TokenType::FALSE },
        { "null", TokenType::NIL },
        { "import", TokenType::IMPORT },
        { "var", TokenType::VAR },
        { "const", TokenType::CONST },
        { "function", TokenType::FUNCTION },
        { "inline", TokenType::INLINE },
        { "return", TokenType::RETURN },
        { "enum", TokenType::ENUM }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(relative_filename, keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
        CHECK(tokens->lens[0] == keyword.length());
    }
}