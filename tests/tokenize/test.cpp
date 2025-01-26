#include <catch2.hpp>
#include "../compiler/lexer/include/lexer.h"

using namespace orx::compiler;

TEST_CASE("Lexer - Basic Tokens")
{
    std::string_view input = "+-*/%;";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens->types.size() == 7); // 6 operators + EOF
    CHECK(tokens->types[0] == TokenType::PLUS);
    CHECK(tokens->types[1] == TokenType::MINUS);
    CHECK(tokens->types[2] == TokenType::STAR);
    CHECK(tokens->types[3] == TokenType::SLASH);
    CHECK(tokens->types[4] == TokenType::PERCENT);
    CHECK(tokens->types[5] == TokenType::SEMICOLON);
}

TEST_CASE("Lexer - Identifiers")
{
    std::string_view input = "abc _test @special";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens->types.size() == 4); // 3 identifiers + EOF
    CHECK(tokens->types[0] == TokenType::IDENTIFIER);
    CHECK(tokens->types[1] == TokenType::IDENTIFIER);
    CHECK(tokens->types[2] == TokenType::IDENTIFIER);
}

TEST_CASE("Lexer - Numbers")
{
    std::string_view input = "123 0xFF 0b1010 3.14 1e-10";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();

    REQUIRE(tokens->types.size() == 6); // 5 numbers + EOF
    for (size_t i = 0; i < 5; i++)
    {
        CHECK(tokens->types[i] == TokenType::NUM_LITERAL);
    }
}

TEST_CASE("Lexer - Comments")
{
    constexpr std::string_view input = "a // comment\nb /* multi\nline */ c";
    Lexer lexer(input);
    const auto tokens = lexer.tokenize();

    REQUIRE(tokens->types.size() == 4);
    CHECK(tokens->types[0] == TokenType::IDENTIFIER);
    CHECK(tokens->types[1] == TokenType::IDENTIFIER);
}

TEST_CASE("Lexer - Error Cases")
{
    SECTION("Invalid identifier")
    {
        std::string_view input = "123abc";
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        CHECK(tokens->types[0] == TokenType::NUM_LITERAL);
    }

    SECTION("Unterminated string")
    {
        std::string_view input = R"("unterminated)";
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        CHECK((static_cast<uint8_t>(tokens->flags[0]) & static_cast<uint8_t>(TokenFlags::UNTERMINATED_STRING)) != 0);
    }

    SECTION("Invalid escape")
    {
        std::string_view input = R"("\y")";
        Lexer lexer(input);
        auto tokens = lexer.tokenize();
        CHECK((static_cast<uint8_t>(tokens->flags[0]) & static_cast<uint8_t>(TokenFlags::INVALID_ESCAPE_SEQUENCE)) != 0)
        ;
    }
}

TEST_CASE("Lexer - Line Tracking")
{
    std::string_view input = "a\nb\nc";
    Lexer lexer(input);
    auto tokens = lexer.tokenize();
    auto line_starts = lexer.get_line_starts();

    REQUIRE(line_starts->size() == 3);
    CHECK((*line_starts)[0] == 0);
    CHECK((*line_starts)[1] == 2);
    CHECK((*line_starts)[2] == 4);
}
