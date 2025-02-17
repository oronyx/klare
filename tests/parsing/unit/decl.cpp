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

TEST_CASE("TEST")
{
    const auto ast = parse_code(R"(
function main() -> u16
{
    var x: u32 = 5;
    const y = 8;
    const z: Ref<u32> = &x;
    return 0;
}
)");

    ast.dump();
}