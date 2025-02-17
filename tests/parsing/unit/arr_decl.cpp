// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/parser/include/parser.h>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

static AST parse_code(const std::string &code)
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, code);
    const auto tokens = lexer.tokenize();
    Parser parser(lexer.module_name, code, *tokens, lexer.get_line_starts());
    auto ast = parser.parse();
    return ast;
}

TEST_CASE("Array infer type")
{
    parse_code("var arr = { 1, 2, 3 };");
}

TEST_CASE("Array with type")
{
    parse_code("var arr: i32[] = { 1, 2, 3 };");
}