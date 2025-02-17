// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/parser/include/parser.h>
#include <compiler/lexer/include/lexer.h>

using namespace klr::compiler;

static AST parse_code(const std::string &code)
{
    Lexer lexer(code);
    const auto tokens = lexer.tokenize();
    Parser parser(*tokens);
    auto ast = parser.parse();
    return ast;
}

TEST_CASE("Array no type")
{
    parse_code("var arr = { 1, 2, 3 };");
}

TEST_CASE("Array with type")
{
    parse_code("var arr: i32[] = { 1, 2, 3 };");
}