// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>

using namespace klr::compiler;

TEST_CASE("Annotations")
{
    std::vector<std::pair<std::string, TokenType> > annotations =
    {
        { "@align", TokenType::ALIGN_ANNOT },
        { "@deprecated", TokenType::DEPRECATED_ANNOT },
        { "@packed", TokenType::PACKED_ANNOT },
        { "@nodiscard", TokenType::NO_DISCARD_ANNOT },
        { "@volatile", TokenType::VOLATILE_ANNOT },
        { "@lazy", TokenType::LAZY_ANNOT },
        { "@pure", TokenType::PURE_ANNOT },
        { "@tailrec", TokenType::TAIL_REC_ANNOT }
    };

    for (const auto &[annotation, type]: annotations)
    {
        Lexer lexer(annotation);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}
