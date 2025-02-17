// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Class")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "class", TokenType::CLASS },
        { "final", TokenType::FINAL },
        { "public", TokenType::PUBLIC },
        { "private", TokenType::PRIVATE },
        { "static", TokenType::STATIC }
    };

    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();
    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(relative_filename, keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}