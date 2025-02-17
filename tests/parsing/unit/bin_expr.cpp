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

TEST_CASE("Bin expr")
{
    SECTION("arithmetic")
    {
        parse_code("var x = 5 + 5;");
        parse_code("var y = 4 - 6;");
        parse_code("var z = 10 * 9;");
        parse_code("var t = 15 / 3;");
        parse_code("var g = 10 % 3;");
    }

    SECTION("bitwise")
    {
        parse_code("var x = 5 & 3;");
        parse_code("var y = 10 | 7;");
        parse_code("var z = 6 ^ 3;");

        /* this gets parsed as '<' & '<'; same with >> case */
        // parse_code("var t = 8 << 2;");
        // parse_code("var g = 16 >> 2;");
    }

    SECTION("cmp")
    {
        parse_code("var x = 5 < 10;");
        parse_code("var y = 10 > 5;");
        parse_code("var z = 5 <= 5;");
        parse_code("var t = 10 >= 10;");
        parse_code("var g = 5 == 5;");
        parse_code("var h = 5 != 6;");
    }

    SECTION("logic")
    {
        parse_code("var x = true && false;");
        parse_code("var y = true || false;");
    }

    SECTION("complex")
    {
        parse_code("var x = (5 + 3) * 2;");
        parse_code("var y = 10 / (2 + 3);");
        parse_code("var z = (5 > 3) && (10 < 15);");
    }

    SECTION("assignment")
    {
        parse_code("var x = 5; x += 3;");
        parse_code("var y = 10; y -= 4;");
        parse_code("var z = 2; z *= 5;");
        parse_code("var t = 20; t /= 4;");
        parse_code("var g = 7; g %= 3;");
    }
}