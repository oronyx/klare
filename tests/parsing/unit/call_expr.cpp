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

TEST_CASE("Call expr")
{
    SECTION("fn call")
    {
        parse_code("print(42);");
        parse_code("calculate(10, 20);");
        parse_code("process();");
    }

    SECTION("call + expr args")
    {
        parse_code("process(5 + 3, calculate(2, 3), true);");
        parse_code("compute((10 * 2), foo(), bar(x));");
        parse_code("handler(complex_func(a, b), 42, \"string\");");
    }

    SECTION("method")
    {
        parse_code("obj.method();");
        parse_code("list.append(42);");
        parse_code("string.substr(0, 5);");
    }

    SECTION("fluent method call")
    {
        parse_code("obj.method1().method2();");
        parse_code("list.first().last().count();");
    }

    SECTION("method + expr args")
    {
        parse_code("object.process(5 + 3, calculate(2, 3), true);");
        parse_code("list.insert(0, compute(10 * 2));");
        parse_code("manager.handle(foo(), 42, \"context\");");
    }

    SECTION("nested call")
    {
        parse_code("outer(inner(x), obj.method(y));");
        parse_code("process(calculate(10, 20), list.get(3));");
    }

    SECTION("generic fn")
    {
        parse_code("generic_func<int>(42);");
        parse_code("generic_method<string, int>(\"hello\", 10);");
    }
}