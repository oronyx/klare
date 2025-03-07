// This file is part of the Klare programming language and is licensed under MIT License;
// See LICENSE.txt for details

#pragma once

#include <memory>
#include <span>
#include "../../interfaces/include/tokens.h"

namespace klr::compiler
{
    class alignas(64) Lexer
    {
    public:
        std::string_view module_name;

        explicit Lexer(std::string_view mod_name, std::string_view src);

        std::unique_ptr<TokenList> tokenize();

        std::unique_ptr<std::vector<uint32_t>> get_line_starts();

    private:
        TokenList tokens;
        std::vector<uint32_t> line_starts;
        std::span<const char> src;
        uint32_t current_pos;
        uint32_t src_length;

        Token next_token();

        void skip_whitespace_comment();

        [[nodiscard]] Token lex_identifier() const;

        [[nodiscard]] Token lex_number() const;

        [[nodiscard]] Token lex_string() const;

        [[nodiscard]] Token lex_operator() const;
    };
}