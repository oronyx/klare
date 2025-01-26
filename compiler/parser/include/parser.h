#pragma once

#include <span>
#include <vector>
#include "../../interfaces/include/tokens.h"

namespace orx::compiler
{
    class alignas(64) Parser
    {
    public:
        Parser(std::string_view src, TokenList tk_list, const std::vector<uint32_t>& line_starts);

        // TBA: AST
        void parse_program();

    private:
        TokenList tokens;
        std::span<const char> src;
        std::vector<uint32_t> line_starts;
        uint32_t current_pos{};
        uint32_t scope{};

        [[nodiscard]] bool is_at_end() const;

        void parse_var_decl();

        void parse_expression();
    };
}
