// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <filesystem>
#include <compiler/lexer/include/lexer.h>

using namespace klr::compiler;

TEST_CASE("EOF")
{std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();
    Lexer lexer(relative_filename, "");
    const auto tks = lexer.tokenize();
    REQUIRE(tks->size() == 1);
}