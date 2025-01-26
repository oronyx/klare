// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include <catch2.hpp>
#include "../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("EOF")
{
    Lexer lexer("");
    const auto tks = lexer.tokenize();
    REQUIRE(tks->size() == 1);
}

TEST_CASE("Keyword")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "true", TokenType::TRUE },
        { "false", TokenType::FALSE },
        { "null", TokenType::NIL },
        { "import", TokenType::IMPORT },
        { "var", TokenType::VAR },
        { "const", TokenType::CONST },
        { "function", TokenType::FUNCTION },
        { "inline", TokenType::INLINE },
        { "return", TokenType::RETURN },
        { "enum", TokenType::ENUM }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
        CHECK(tokens->lens[0] == keyword.length());
    }
}

TEST_CASE("Control flow")
{
    std::vector<std::pair<std::string, TokenType> > keywords = {
        { "if", TokenType::IF },
        { "else", TokenType::ELSE },
        { "for", TokenType::FOR },
        { "while", TokenType::WHILE },
        { "break", TokenType::BREAK },
        { "continue", TokenType::CONTINUE },
        { "switch", TokenType::SWITCH },
        { "case", TokenType::CASE },
        { "default", TokenType::DEFAULT }
    };

    for (const auto &[keyword, type]: keywords)
    {
        Lexer lexer(keyword);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}

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

TEST_CASE("Builtin types")
{
    std::vector<std::pair<std::string, TokenType> > types = {
        { "u8", TokenType::U8 },
        { "i8", TokenType::I8 },
        { "u16", TokenType::U16 },
        { "i16", TokenType::I16 },
        { "u32", TokenType::U32 },
        { "i32", TokenType::I32 },
        { "u64", TokenType::U64 },
        { "i64", TokenType::I64 },
        { "f32", TokenType::F32 },
        { "f64", TokenType::F64 },
        { "string", TokenType::STRING },
        { "bool", TokenType::BOOL },
        { "void", TokenType::VOID },
        { "Own", TokenType::OWN },
        { "Share", TokenType::SHARE },
        { "Ref", TokenType::REF },
        { "Pin", TokenType::PIN }
    };

    for (const auto &[type_name, type]: types)
    {
        Lexer lexer(type_name);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}

TEST_CASE("Single-character operators")
{
    std::vector<std::pair<std::string, TokenType> > operators = {
        { "+", TokenType::PLUS },
        { "-", TokenType::MINUS },
        { "*", TokenType::STAR },
        { "/", TokenType::SLASH },
        { "%", TokenType::PERCENT },
        { "=", TokenType::EQUAL },
        { "!", TokenType::BANG },
        { "<", TokenType::LESS },
        { ">", TokenType::GREATER },
        { "&", TokenType::AND },
        { "|", TokenType::OR },
        { "^", TokenType::XOR },
        { "~", TokenType::TILDE },
        { ".", TokenType::DOT }
    };

    for (const auto &[op, type]: operators)
    {
        Lexer lexer(op);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}

TEST_CASE("Multi-character operators")
{
    std::vector<std::pair<std::string, TokenType> > operators = {
        { "->", TokenType::ARROW },
        { "::", TokenType::SCOPE },
        { "..", TokenType::RANGE },
        { "...", TokenType::SPREAD },
        { "&&", TokenType::LOGICAL_AND },
        { "||", TokenType::LOGICAL_OR },
        { ">=", TokenType::GE },
        { "<=", TokenType::LE },
        { "==", TokenType::EQ },
        { "!=", TokenType::NE },
        { "+=", TokenType::PLUS_EQ },
        { "-=", TokenType::MINUS_EQ },
        { "*=", TokenType::STAR_EQ },
        { "/=", TokenType::SLASH_EQ },
        { "%=", TokenType::PERCENT_EQ },
        { "&=", TokenType::AND_EQ },
        { "|=", TokenType::OR_EQ },
        { "^=", TokenType::XOR_EQ },
        { "<<", TokenType::LEFT_SHIFT },
        { ">>", TokenType::RIGHT_SHIFT },
        { "<<=", TokenType::LEFT_SHIFT_EQ },
        { ">>=", TokenType::RIGHT_SHIFT_EQ }
    };

    for (const auto &[op, type]: operators)
    {
        Lexer lexer(op);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}

TEST_CASE("Delimiters")
{
    std::vector<std::pair<std::string, TokenType> > delimiters = {
        { "(", TokenType::LEFT_PAREN },
        { ")", TokenType::RIGHT_PAREN },
        { "{", TokenType::LEFT_BRACE },
        { "}", TokenType::RIGHT_BRACE },
        { "[", TokenType::LEFT_BRACKET },
        { "]", TokenType::RIGHT_BRACKET },
        { ",", TokenType::COMMA },
        { ":", TokenType::COLON },
        { ";", TokenType::SEMICOLON },
        { "?", TokenType::QUESTION }
    };

    for (const auto &[delimiter, type]: delimiters)
    {
        Lexer lexer(delimiter);
        const auto tokens = lexer.tokenize();
        REQUIRE(tokens->size() == 2);
        CHECK(tokens->types[0] == type);
    }
}

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

TEST_CASE("Identifier")
{
    SECTION("Valid")
    {
        std::vector<std::string> identifiers = {
            "foo",
            "bar123",
            "_private",
            "camelCase",
            "snake_case",
            "SCREAMING_SNAKE",
        };

        for (const auto &id: identifiers)
        {
            Lexer lexer(id);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            // std::cout << token_to_str(tokens->types[0]) << "\n";
            CHECK(tokens->types[0] == TokenType::IDENTIFIER);
            CHECK(tokens->lens[0] == id.length());
        }
    }

    SECTION("Invalid")
    {
        std::vector<std::string> invalid_identifiers = {
            "123abc",
            "@invalid",
            "αβγ",
        };

        for (const auto &id: invalid_identifiers)
        {
            Lexer lexer(id);
            const auto tokens = lexer.tokenize();
            // std::cout << token_to_str(tokens->types[0]) << "\n";
            CHECK(tokens->types[0] == TokenType::UNKNOWN);
        }
    }
}

TEST_CASE("Number literals")
{
    SECTION("Integer literals")
    {
        std::vector<std::string> integers = {
            "0",
            "123",
            "0xFF",   // hex
            "0b1010", // bin
        };

        for (const auto &num: integers)
        {
            Lexer lexer(num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }

    SECTION("Float literals")
    {
        std::vector<std::string> floats = {
            "0.0",
            "123.456",
            "1e10",
            "1.23e-4",
        };

        for (const auto &num: floats)
        {
            Lexer lexer(num);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
            CHECK(tokens->lens[0] == num.length());
        }
    }
}

TEST_CASE("String literals")
{
    SECTION("Basic strings")
    {
        std::vector<std::string> strings = {
            "\"Hello, world!\"",
            "\"\"",  // empty
            "\"123\"",
            "\"Special chars: !@#$%^&*()\""
        };

        for (const auto& str : strings)
        {
            Lexer lexer(str);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::STR_LITERAL);
            CHECK(tokens->lens[0] == str.length());
        }
    }

    SECTION("Strings with escapes")
    {
        std::vector<std::string> strings = {
            R"("Escaped quotes: \"")",
            R"("Newline: \n")",
            R"("Tab: \t")",
            R"("Unicode: \u1234")"
        };

        for (const auto& str : strings)
        {
            Lexer lexer(str);
            const auto tokens = lexer.tokenize();
            REQUIRE(tokens->size() == 2);
            CHECK(tokens->types[0] == TokenType::STR_LITERAL);
        }
    }
}


TEST_CASE("Comments")
{
    SECTION("Single")
    {
        Lexer lexer(R"(// This is a single-line comment)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }

    SECTION("Multi")
    {
        Lexer lexer(R"(/*
        This is a multi-line comment
        TEST
        TEST
        TEST
        TEST
        TEST
        */
)");
        const auto tokens = lexer.tokenize();
        CHECK(tokens->size() == 1);
    }
}