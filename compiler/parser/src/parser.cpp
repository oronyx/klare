#include <utility>
#include "../include/parser.h"

namespace orx::compiler
{
    Parser::Parser(std::string_view src, TokenList tk_list, const std::vector<uint32_t>& line_starts) : tokens(std::move(tk_list)),
                                                                                         line_starts(line_starts), src(src)
    {

    }

    void Parser::parse_program()
    {
        while (!is_at_end())
        {
            switch (tokens.types[current_pos])
            {
                case TokenType::VAR:
                case TokenType::CONST:
                {
                    parse_var_decl();
                }
                default:
                    parse_expression();
            }
        }
    }

    bool Parser::is_at_end() const
    {
        return current_pos >= tokens.size() || tokens.types[current_pos] == TokenType::END_OF_FILE;
    }

    void Parser::parse_var_decl()
    {
        auto is_const = tokens.types[current_pos] == TokenType::CONST;
        auto is_var = tokens.types[current_pos] == TokenType::VAR;
        current_pos++;

        // TODO: if is_const then mark the item in AST as const
        if (tokens.types[current_pos] != TokenType::IDENTIFIER)
        {
            // TODO: ERROR: proper expected identifier
            throw std::runtime_error("expected identifier");
        }

        std::string_view var_name(
            src.data() + tokens.starts[current_pos],
            tokens.lens[current_pos]
        );
        current_pos++;

        if (tokens.types[current_pos] == TokenType::COLON)
        {
            current_pos++;
            if (tokens.types[current_pos] != TokenType::IDENTIFIER)
            {
                // TODO: ERROR: proper expected type
                throw std::runtime_error("expected type");
            }

            std::string_view var_type(
                src.data() + tokens.starts[current_pos],
                tokens.lens[current_pos]
            );
            current_pos++;
        }
        // TODO: add tag to type system queue for infer

        if (tokens.types[current_pos] == TokenType::EQUAL)
        {
            current_pos++;
            parse_expression();
        }

        // TODO: Add to symbol table
    }

    void Parser::parse_expression()
    {
        while (!is_at_end() &&
               tokens.types[current_pos] != TokenType::SEMICOLON &&
               tokens.types[current_pos] != TokenType::END_OF_FILE)
        {
            current_pos++;
        }

        if (tokens.types[current_pos] == TokenType::SEMICOLON)
            current_pos++;
    }
}
