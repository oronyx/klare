// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"
#include "../../../compiler/parser/include/parser.h"

using namespace orx::compiler;

std::unique_ptr<Parser> create_parser(const std::string& input)
{
    static std::vector source(input.begin(), input.end());
    const std::span<const char> src(source);
    Lexer lexer((src.data()));
    const auto tokens = lexer.tokenize();
    const auto line_starts = lexer.get_line_starts();
    return std::make_unique<Parser>(input, *tokens, *line_starts);
}

TEST_CASE("Simple variable declaration")
{
    const auto parser = create_parser("var x = 42;");
    REQUIRE_NOTHROW(parser->parse_program());
}

TEST_CASE("Variable declaration with type")
{
    const auto parser = create_parser("var x: u8 = 42;");
    REQUIRE_NOTHROW(parser->parse_program());
}

TEST_CASE("Constant declaration")
{
    const auto parser = create_parser("const y = 100;");
    REQUIRE_NOTHROW(parser->parse_program());
}
