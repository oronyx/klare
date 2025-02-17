// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include <compiler/lexer/include/lexer.h>
#include <filesystem>

using namespace klr::compiler;

TEST_CASE("Complex")
{
    std::string test_name = Catch::getResultCapture().getCurrentTestName();
    std::filesystem::path test_file_path = std::filesystem::current_path() / (test_name + ".klr");
    std::string relative_filename = test_file_path.string();

    Lexer lexer(relative_filename, R"(
        @packed(4)
        struct Vector
        {
            var x: f32;
            var y: f32;
        }
    )");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::PACKED_ANNOT,
          TokenType::LEFT_PAREN,
          TokenType::NUM_LITERAL,
          TokenType::RIGHT_PAREN,
          TokenType::STRUCT,
          TokenType::IDENTIFIER,
          TokenType::LEFT_BRACE,
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::F32,
          TokenType::SEMICOLON,
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::F32,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}
