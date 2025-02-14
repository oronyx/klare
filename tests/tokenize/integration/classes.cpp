// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>

#include "../../../compiler/lexer/include/lexer.h"

using namespace klr::compiler;

TEST_CASE("Class definition")
{
    Lexer lexer(R"(
       class Point<T>
       {
           var x: T;
           var y: T;

           public operator new(x: T, y: T) -> Point<T>
           {
               self.x = x;
               self.y = y;
           }
       }
   )");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::CLASS,
          TokenType::IDENTIFIER,
          TokenType::LESS,
          TokenType::IDENTIFIER,
          TokenType::GREATER,
          TokenType::LEFT_BRACE,
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::VAR,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::PUBLIC,
          TokenType::OPERATOR,
          TokenType::NEW,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::IDENTIFIER,
          TokenType::COMMA,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::IDENTIFIER,
          TokenType::RIGHT_PAREN,
          TokenType::ARROW,
          TokenType::IDENTIFIER,
          TokenType::LESS,
          TokenType::IDENTIFIER,
          TokenType::GREATER,
          TokenType::LEFT_BRACE,
          TokenType::SELF,
          TokenType::DOT,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::SELF,
          TokenType::DOT,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}

TEST_CASE("Class inheritance")
{
    Lexer lexer(R"(
       class Rectangle : Shape
       {
           var width: f32;
           var height: f32;

           public operator new(w: f32, h: f32) -> Rectangle
           {
               self.width = w;
               self.height = h;
           }

           @override
           function area() -> f32
           {
               return width * height;
           }
       }
   )");
    const auto tks = lexer.tokenize();
    CHECK(tks->types == std::vector<TokenType>{
          TokenType::CLASS,
          TokenType::IDENTIFIER,
          TokenType::COLON,
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
          TokenType::PUBLIC,
          TokenType::OPERATOR,
          TokenType::NEW,
          TokenType::LEFT_PAREN,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::F32,
          TokenType::COMMA,
          TokenType::IDENTIFIER,
          TokenType::COLON,
          TokenType::F32,
          TokenType::RIGHT_PAREN,
          TokenType::ARROW,
          TokenType::IDENTIFIER,
          TokenType::LEFT_BRACE,
          TokenType::SELF,
          TokenType::DOT,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::SELF,
          TokenType::DOT,
          TokenType::IDENTIFIER,
          TokenType::EQUAL,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::OVERRIDE_ANNOT,
          TokenType::FUNCTION,
          TokenType::IDENTIFIER,
          TokenType::LEFT_PAREN,
          TokenType::RIGHT_PAREN,
          TokenType::ARROW,
          TokenType::F32,
          TokenType::LEFT_BRACE,
          TokenType::RETURN,
          TokenType::IDENTIFIER,
          TokenType::STAR,
          TokenType::IDENTIFIER,
          TokenType::SEMICOLON,
          TokenType::RIGHT_BRACE,
          TokenType::RIGHT_BRACE,
          TokenType::END_OF_FILE
          });
}
