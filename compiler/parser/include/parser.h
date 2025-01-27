// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#pragma once

#include <span>
#include <vector>
#include "symbol.h"
#include "../../ast/include/ast.h"
#include "../../interfaces/include/tokens.h"

namespace orx::compiler
{
    struct ParseError
    {
        std::string_view msg;
        uint32_t pos;
        uint32_t line;
        uint32_t col;
    };

    class alignas(64) Parser
    {
    public:
        Parser(std::string_view src, TokenList tk_list, const std::vector<uint32_t> &line_starts);

        // TBA: AST
        std::unique_ptr<AST> parse_program();

        uint32_t parse_type();

    private:
        TokenList tokens;
        AST ast {};
        std::vector<ParseError> errors;
        std::vector<Symbol> symbols;
        std::vector<uint32_t> scope_boundaries;
        std::vector<uint32_t> line_starts;
        std::span<const char> src;
        uint32_t current_pos {};
        uint32_t scope {};

        void parse_var_decl();

        auto add_type(std::string_view type_name) -> uint32_t;

        void parse_expression();

        void parse_primary();

        void parse_function_call(std::string_view func_name);

        void parse_array_access(std::string_view array_name);

        void parse_lambda();

        void parse_unary();

        void parse_function();

        void parse_statement();

        void parse_if_statement();

        void parse_while_statement();

        void parse_for_statement();

        void parse_switch_statement();

        void parse_return_statement();

        void parse_binary(uint8_t precedence);

        void parse_module();

        // symbol table methods
        void add_symbol(std::string_view name, Symbol::Type type, uint32_t type_idx);

        [[nodiscard]] bool symbol_exists(std::string_view name) const;

        void enter_scope();

        void exit_scope();

        Symbol* get_symbol(std::string_view name);

        std::pair<uint32_t, uint32_t> get_line_col(const Token &token);

        [[nodiscard]] bool is_at_end() const;
    };
}
