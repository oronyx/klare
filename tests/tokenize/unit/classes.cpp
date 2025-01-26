// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Class")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "class", TokenType::CLASS },
        { "final", TokenType::FINAL },
        { "public", TokenType::PUBLIC },
        { "private", TokenType::PRIVATE },
        { "static", TokenType::STATIC }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}