// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <filesystem>
#include <compiler/lexer/include/lexer.h>
#include <compiler/parser/include/parser.h>

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

TEST_CASE("unary expr")
{
    SECTION("logical not")
    {
        parse_code("var x = -5;");
        parse_code("var y = !true;");
        parse_code("var z = -(10 + 5);");
    }

    SECTION("bnot")
    {
        parse_code("var x = ~42;");
        parse_code("var y = ~(10 & 5);");
    }

    SECTION("ref & deref")
    {
        parse_code("var x = &y;");
        parse_code("var z = *ptr;");
    }

    SECTION("mm")
    {
        parse_code("var x: i32 = new i32(5);");
        parse_code("function mm() -> void { delete x; }");
    }

    SECTION("complex")
    {
        parse_code("var x = -(5 + 3) * 2;");
        parse_code("var y = !false && true;");
        parse_code("var z = *ptr + 5;");
    }
}