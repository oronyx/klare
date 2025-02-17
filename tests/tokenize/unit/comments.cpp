// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Comments")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    SECTION("Single")
    {
        Lexer lexer(relative_filename, R"(// This is a single-line comment)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }

    SECTION("Multi")
    {
        Lexer lexer(relative_filename, R"(/*
        This is a multi-line comment
        TEST
        TEST
        TEST
        TEST
        TEST
        */
)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }
}