#pragma once

#include <vector>
#include <compiler/interfaces/include/tokens.h>
#include <compiler/analysis/include/ast.h>

namespace klr::compiler
{
    /* TODO: change void type to AST */
    class Parser
    {
    public:
        explicit Parser(TokenList &tokens);

        /* entry point & global scope parsing; returns the AST root */
        AST parse();

    private:
        AST ast;
        TokenList &tokens;
        size_t current = 0;

        /* parsing utils */
        [[nodiscard]] Token peek() const;

        Token advance();

        [[nodiscard]] bool is_at_end() const;

        bool match(TokenType type);

        Token expect(TokenType type);

        /* subroutines */
        uint32_t parse_decl();

        uint32_t parse_function(bool is_lambda);

        uint32_t parse_type();

        uint32_t parse_primary();

        uint32_t parse_expression();

        /* parse_expression subroutines */
        uint32_t parse_assignment();

        uint32_t parse_ternary();

        uint32_t parse_logical_or();

        uint32_t parse_logical_and();

        uint32_t parse_bitwise_or();

        uint32_t parse_bitwise_xor();

        uint32_t parse_bitwise_and();

        uint32_t parse_shift();

        uint32_t parse_equality();

        uint32_t parse_comparison();

        uint32_t parse_term();

        uint32_t parse_factor();

        uint32_t parse_unary();

        /*                              */

        uint32_t parse_if();

        uint32_t parse_for();

        uint32_t parse_block(Token brace);

        uint32_t parse_while();
    };
}
