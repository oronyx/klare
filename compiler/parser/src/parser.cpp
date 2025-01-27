// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include "../include/parser.h"

#include <ranges>
#include <thread>
#include <unordered_set>

namespace orx::compiler
{
    Parser::Parser(std::string_view src, TokenList tk_list,
                   const std::vector<uint32_t> &line_starts) : tokens(std::move(tk_list)),
                                                               line_starts(line_starts), src(src) {}

    std::unique_ptr<AST> Parser::parse_program()
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
        return std::make_unique<AST>(ast);
    }

    uint32_t Parser::parse_type()
    {
        switch (tokens.types[current_pos])
        {
            case TokenType::U8:
            case TokenType::I8:
            case TokenType::U16:
            case TokenType::I16:
            case TokenType::U32:
            case TokenType::I32:
            case TokenType::U64:
            case TokenType::I64:
            case TokenType::F32:
            case TokenType::F64:
            case TokenType::STRING:
            case TokenType::BOOL:
            case TokenType::VOID:
            {
                uint32_t base_type_idx = add_type(
                    std::string(src.data() + tokens.starts[current_pos],
                                tokens.lens[current_pos])
                );
                current_pos++;
                return base_type_idx;
            }
            default:
                break;
        }

        if (tokens.types[current_pos] == TokenType::IDENTIFIER)
        {
            std::string_view type_name(src.data() + tokens.starts[current_pos],
                                       tokens.lens[current_pos]);

            if (type_name == "Own" || type_name == "Share" ||
                type_name == "Ref" || type_name == "Pin")
            {
                current_pos++;
                if (tokens.types[current_pos] != TokenType::LESS)
                    throw std::runtime_error("Expected < for generic type");
                current_pos++;

                parse_type();
                if (tokens.types[current_pos] != TokenType::GREATER)
                    throw std::runtime_error("Expected > after generic type");
                current_pos++;

                return add_type(std::string(type_name) + "<inner_type>");
            }
        }

        uint32_t base_type_idx = add_type(
            std::string(src.data() + tokens.starts[current_pos],
                        tokens.lens[current_pos])
        );
        current_pos++;

        while (tokens.types[current_pos] == TokenType::LEFT_BRACKET)
        {
            current_pos++;
            if (tokens.types[current_pos] != TokenType::RIGHT_BRACKET)
                throw std::runtime_error("Expected ] for array type");
            current_pos++;

            base_type_idx = add_type("array<" + std::string(ast.strings[base_type_idx].data()) + ">");
        }

        return base_type_idx;
    }

    void Parser::parse_var_decl()
    {
        const auto is_const = tokens.types[current_pos] == TokenType::CONST;
        current_pos++;

        if (tokens.types[current_pos] != TokenType::IDENTIFIER)
            throw std::runtime_error("expected identifier");

        const std::string_view var_name(src.data() + tokens.starts[current_pos],
                                        tokens.lens[current_pos]);
        current_pos++;

        uint32_t type_idx = 0;
        if (tokens.types[current_pos] == TokenType::COLON)
        {
            current_pos++;
            type_idx = parse_type();
        }

        if (tokens.types[current_pos] == TokenType::EQUAL)
        {
            current_pos++;
            parse_expression();
        }

        add_symbol(var_name, is_const ? Symbol::Type::CONST : Symbol::Type::VAR, type_idx);
    }

    uint32_t Parser::add_type(std::string_view type_name)
    {
        for (size_t i = 0; i < ast.types.size(); i++)
        {
            if (ast.strings[ast.types[i]] == type_name)
                return i;
        }
        ast.types.emplace_back(ast.strings.size());
        ast.strings.emplace_back(type_name);
        return ast.types.size() - 1;
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

    void Parser::parse_primary()
    {
        std::string_view token_text(src.data() + tokens.starts[current_pos],
                                    tokens.lens[current_pos]);

        switch (tokens.types[current_pos])
        {
            case TokenType::NUM_LITERAL:
            case TokenType::STR_LITERAL:
            {
                LiteralExpr literal {};
                literal.value_idx = ast.strings.size();
                literal.type_tag = 0;
                ast.strings.emplace_back(token_text);

                Expr expr(ExprType::LITERAL, tokens.starts[current_pos], literal);
                ast.nodes.emplace_back(expr);
                current_pos++;
                break;
            }
            case TokenType::IDENTIFIER:
            {
                if (get_symbol(token_text))
                {
                    // Function call or variable
                    current_pos++;
                    if (tokens.types[current_pos] == TokenType::LEFT_PAREN)
                    {
                        parse_function_call(token_text);
                    }
                    else if (tokens.types[current_pos] == TokenType::LEFT_BRACKET)
                    {
                        parse_array_access(token_text);
                    }
                    else
                    {
                        LiteralExpr literal {};
                        literal.value_idx = ast.strings.size();
                        literal.type_tag = 0;

                        Expr expr(ExprType::LITERAL, tokens.starts[current_pos], literal);
                        ast.nodes.emplace_back(expr);
                    }
                }
                break;
            }
            case TokenType::FUNCTION:
            {
                parse_lambda();
                break;
            }
            case TokenType::LEFT_PAREN:
            {
                current_pos++;
                parse_expression();
                if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
                    throw std::runtime_error("Expected )");
                current_pos++;
                break;
            }
            default:
                throw std::runtime_error("Unexpected token");
        }
    }

    void Parser::parse_function_call(std::string_view func_name)
    {
        current_pos++;
        std::vector<uint32_t> arg_indices;

        while (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
        {
            parse_expression();
            arg_indices.push_back(ast.nodes.size() - 1);

            if (tokens.types[current_pos] == TokenType::COMMA)
                current_pos++;
        }
        current_pos++;
        CallExpr call_data{0, std::move(arg_indices)};
        Expr expr(ExprType::CALL, tokens.starts[current_pos], call_data);
        ast.nodes.emplace_back(expr);
    }

    void Parser::parse_array_access(std::string_view array_name)
    {
        current_pos++;
        parse_expression();

        if (tokens.types[current_pos] != TokenType::RIGHT_BRACKET)
            throw std::runtime_error("Expected ] after array index");
        current_pos++;

        Expr expr(ExprType::INDEX, tokens.starts[current_pos], ArrayAccessExpr(0ULL, ast.nodes.size() - 1));
        ast.nodes.emplace_back(expr);
    }

    void Parser::parse_lambda()
    {
        current_pos++;
        enter_scope();

        if (tokens.types[current_pos] != TokenType::LEFT_PAREN)
            throw std::runtime_error("Expected ( for lambda parameters");
        current_pos++;

        std::vector<std::tuple<std::string_view, uint32_t, bool> > params;
        while (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
        {
            if (tokens.types[current_pos] != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected parameter name");

            std::string_view param_name(src.data() + tokens.starts[current_pos],
                                        tokens.lens[current_pos]);
            current_pos++;

            if (tokens.types[current_pos] != TokenType::COLON)
                throw std::runtime_error("Expected : for parameter type");
            current_pos++;

            uint32_t type_idx = parse_type();
            add_symbol(param_name, Symbol::Type::VAR, type_idx);
            params.emplace_back(param_name, type_idx, false);

            if (tokens.types[current_pos] == TokenType::COMMA)
                current_pos++;
        }
        current_pos++;
        if (tokens.types[current_pos] != TokenType::ARROW)
            throw std::runtime_error("Expected -> for return type");
        current_pos++;

        uint32_t return_type_idx = parse_type();

        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for lambda body");
        current_pos++;

        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
            parse_statement();

        current_pos++;
        exit_scope();

        Expr expr(ExprType::LAMBDA, tokens.starts[current_pos], LambdaExpr(
            return_type_idx,
            std::move(params)
        ));
        ast.nodes.emplace_back(expr);
    }

    void Parser::parse_unary()
    {
        if (tokens.types[current_pos] == TokenType::MINUS ||
            tokens.types[current_pos] == TokenType::BANG)
        {
            auto op = tokens.types[current_pos];
            const uint32_t op_pos = tokens.starts[current_pos];
            current_pos++;
            parse_primary();

            Expr expr(ExprType::UNARY, op_pos, UnaryExpr(static_cast<uint32_t>(ast.nodes.size() - 1), static_cast<uint8_t>(op)));
            ast.nodes.emplace_back(expr);
        }
        else
        {
            parse_primary();
        }
    }

    void Parser::parse_function()
    {
        current_pos++;
        if (tokens.types[current_pos] != TokenType::IDENTIFIER)
            throw std::runtime_error("Expected function name");

        const std::string_view func_name(src.data() + tokens.starts[current_pos],
                                         tokens.lens[current_pos]);
        current_pos++;

        // generics
        if (tokens.types[current_pos] == TokenType::LESS)
        {
            current_pos++;
            while (tokens.types[current_pos] != TokenType::GREATER)
            {
                if (tokens.types[current_pos] != TokenType::IDENTIFIER)
                    throw std::runtime_error("Expected generic type parameter");

                const std::string_view generic_param(
                    src.data() + tokens.starts[current_pos],
                    tokens.lens[current_pos]
                );
                uint32_t generic_type_idx = add_type(generic_param);
                ast.generic_types.push_back(generic_type_idx);

                current_pos++;
                if (tokens.types[current_pos] == TokenType::COMMA)
                    current_pos++;
            }
            current_pos++;
        }

        // fn params
        if (tokens.types[current_pos] != TokenType::LEFT_PAREN)
            throw std::runtime_error("Expected ( for function parameters");
        current_pos++;

        enter_scope();
        std::vector<std::tuple<std::string_view, uint32_t, bool> > params;
        bool is_variadic = false;

        while (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
        {
            is_variadic = tokens.types[current_pos] == TokenType::SPREAD;
            if (is_variadic)
                current_pos++;

            if (tokens.types[current_pos] != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected parameter name");

            std::string_view param_name(src.data() + tokens.starts[current_pos],
                                        tokens.lens[current_pos]);
            current_pos++;

            if (tokens.types[current_pos] != TokenType::COLON)
                throw std::runtime_error("Expected : for parameter type");
            current_pos++;

            if (tokens.types[current_pos] != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected parameter type");

            std::string_view param_type(src.data() + tokens.starts[current_pos],
                                        tokens.lens[current_pos]);
            uint32_t type_idx = add_type(param_type);
            current_pos++;
            add_symbol(param_name, Symbol::Type::VAR, type_idx);
            params.emplace_back(param_name, type_idx, is_variadic);

            if (tokens.types[current_pos] == TokenType::COMMA)
                current_pos++;
        }
        current_pos++;

        // return
        if (tokens.types[current_pos] != TokenType::ARROW)
            throw std::runtime_error("Expected -> for return type");
        current_pos++;

        if (tokens.types[current_pos] != TokenType::IDENTIFIER)
            throw std::runtime_error("Expected return type");

        const std::string_view return_type(src.data() + tokens.starts[current_pos],
                                           tokens.lens[current_pos]);
        const uint32_t return_type_idx = add_type(return_type);
        current_pos++;

        // fn body
        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for function body");
        current_pos++;

        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
            parse_statement();

        current_pos++;
        exit_scope();
        add_symbol(func_name, Symbol::Type::FUNCTION, return_type_idx);
    }

    void Parser::parse_statement()
    {
        switch (tokens.types[current_pos])
        {
            case TokenType::RETURN:
                parse_return_statement();
                break;
            case TokenType::IF:
                parse_if_statement();
                break;
            case TokenType::WHILE:
                parse_while_statement();
                break;
            case TokenType::FOR:
                parse_for_statement();
                break;
            case TokenType::SWITCH:
                parse_switch_statement();
                break;
            default:
                parse_expression();
        }
    }

    void Parser::parse_if_statement()
    {
        current_pos++;
        if (tokens.types[current_pos] != TokenType::LEFT_PAREN)
            throw std::runtime_error("Expected ( after if");
        current_pos++;

        parse_expression(); // condition
        if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
            throw std::runtime_error("Expected ) after condition");
        current_pos++;

        enter_scope();
        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for if body");
        current_pos++;

        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
            parse_statement();
        current_pos++;
        exit_scope();

        if (tokens.types[current_pos] == TokenType::ELSE)
        {
            current_pos++;
            enter_scope();
            if (tokens.types[current_pos] == TokenType::IF)
                parse_if_statement();
            else
            {
                if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
                    throw std::runtime_error("Expected { for else body");
                current_pos++;

                while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
                    parse_statement();
                current_pos++;
            }
            exit_scope();
        }
    }

    void Parser::parse_while_statement()
    {
        current_pos++;
        if (tokens.types[current_pos] != TokenType::LEFT_PAREN)
            throw std::runtime_error("Expected ( after while");
        current_pos++;

        parse_expression(); // condition

        if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
            throw std::runtime_error("Expected ) after condition");
        current_pos++;

        enter_scope();
        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for while body");
        current_pos++;

        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
            parse_statement();
        current_pos++;
        exit_scope();
    }

    void Parser::parse_for_statement()
    {
        current_pos++;
        enter_scope();

        if (tokens.types[current_pos] == TokenType::LEFT_PAREN)
        {
            current_pos++;
            if (tokens.types[current_pos] != TokenType::SEMICOLON)
                parse_expression();

            if (tokens.types[current_pos] != TokenType::SEMICOLON)
                throw std::runtime_error("Expected ; after init");
            current_pos++;

            // cond
            if (tokens.types[current_pos] != TokenType::SEMICOLON)
                parse_expression();

            if (tokens.types[current_pos] != TokenType::SEMICOLON)
                throw std::runtime_error("Expected ; after condition");
            current_pos++;

            // inc
            if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
                parse_expression();

            if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
                throw std::runtime_error("Expected ) after for clauses");
            current_pos++;
        }
        else
        {
            // range-based iter
            if (tokens.types[current_pos] != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected identifier in for loop");
            current_pos++;

            if (tokens.types[current_pos] != TokenType::IN)
                throw std::runtime_error("Expected 'in' in for loop");
            current_pos++;

            parse_expression(); // iterable
        }

        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for for body");
        current_pos++;

        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
            parse_statement();
        current_pos++;
        exit_scope();
    }

    void Parser::parse_switch_statement()
    {
        current_pos++;
        if (tokens.types[current_pos] != TokenType::LEFT_PAREN)
            throw std::runtime_error("Expected ( after switch");
        current_pos++;

        parse_expression(); // switch
        if (tokens.types[current_pos] != TokenType::RIGHT_PAREN)
            throw std::runtime_error("Expected ) after switch expression");
        current_pos++;

        if (tokens.types[current_pos] != TokenType::LEFT_BRACE)
            throw std::runtime_error("Expected { for switch body");
        current_pos++;

        enter_scope();
        while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
        {
            switch (tokens.types[current_pos])
            {
                case TokenType::CASE:
                    current_pos++;
                    parse_expression(); // case value
                    if (tokens.types[current_pos] != TokenType::COLON)
                        throw std::runtime_error("Expected : after case");
                    current_pos++;
                    while (tokens.types[current_pos] != TokenType::CASE &&
                           tokens.types[current_pos] != TokenType::DEFAULT &&
                           tokens.types[current_pos] != TokenType::RIGHT_BRACE)
                        parse_statement();
                    break;
                case TokenType::DEFAULT:
                    current_pos++;
                    if (tokens.types[current_pos] != TokenType::COLON)
                        throw std::runtime_error("Expected : after default");
                    current_pos++;
                    while (tokens.types[current_pos] != TokenType::RIGHT_BRACE)
                        parse_statement();
                    break;
                default:
                    throw std::runtime_error("Expected case or default in switch");
            }
        }
        current_pos++;
        exit_scope();
    }

    void Parser::parse_return_statement()
    {
        current_pos++;
        parse_expression();

        if (tokens.types[current_pos] != TokenType::SEMICOLON)
            throw std::runtime_error("Expected ; after return statement");
        current_pos++;
    }

    static uint8_t get_precedence(TokenType type)
    {
        switch (type)
        {
            case TokenType::STAR:
            case TokenType::SLASH:
                return 5;
            case TokenType::PLUS:
            case TokenType::MINUS:
                return 4;
            case TokenType::LESS:
            case TokenType::GREATER:
                return 3;
            case TokenType::EQ:
                return 2;
            case TokenType::AND:
            case TokenType::OR:
                return 1;
            default:
                return 0;
        }
    }

    void Parser::parse_binary(uint8_t precedence)
    {
        parse_unary();
        while (precedence <= get_precedence(tokens.types[current_pos]))
        {
            if (tokens.types[current_pos] == TokenType::QUESTION)
            {
                current_pos++;
                parse_expression(); // true br
                if (tokens.types[current_pos] != TokenType::COLON)
                    throw std::runtime_error("Expected : in ternary");
                current_pos++;
                parse_expression(); // false br

                Expr expr(ExprType::CONDITIONAL, tokens.starts[current_pos], ConditionalExpr(
                    ast.nodes.size() - 3,
                    ast.nodes.size() - 2,
                    ast.nodes.size() - 1
                ));
                ast.nodes.emplace_back(expr);
            }
            else
            {
                auto op = tokens.types[current_pos];
                current_pos++;

                const size_t left = ast.nodes.size() - 1;
                parse_binary(get_precedence(op) + 1);
                const size_t right = ast.nodes.size() - 1;


                Expr expr(ExprType::BINARY, tokens.starts[current_pos], BinaryExpr(
                    static_cast<uint32_t>(left),
                    static_cast<uint32_t>(right),
                    static_cast<uint8_t>(op)
                ));
                ast.nodes.emplace_back(expr);
            }
        }
    }

    void Parser::parse_module()
    {
        std::vector<std::thread> parsers;
        std::unordered_set<std::string_view> parsed_modules;
        while (!is_at_end())
        {
            if (tokens.types[current_pos] == TokenType::IMPORT)
            {
                current_pos++;
                if (tokens.types[current_pos] != TokenType::STR_LITERAL)
                {
                    throw std::runtime_error("Expected module path");
                }

                std::string_view module_path(src.data() + tokens.starts[current_pos],
                                             tokens.lens[current_pos]);

                if (!parsed_modules.contains(module_path))
                {
                    parsers.emplace_back([this, module_path]()
                    {
                        Symbol module_sym {
                            .type = Symbol::Type::MODULE,
                            .name_index = static_cast<uint32_t>(ast.strings.size()),
                            .type_idx = 0,
                            .scope_level = 0, // global scope
                            .flags = 0
                        };
                        symbols.push_back(module_sym);
                        ast.strings.push_back(module_path);
                    });
                    parsed_modules.insert(module_path);
                }
                current_pos++;
            }
        }

        for (auto &parser: parsers)
            parser.join();
    }

    void Parser::add_symbol(std::string_view name, Symbol::Type type, uint32_t type_idx)
    {
        // TODO: actual error handling
        if (symbol_exists(name))
        {
            throw std::runtime_error("Symbol already defined");
        }

        const Symbol sym {
            .type = type,
            .name_index = static_cast<uint32_t>(ast.strings.size()),
            .type_idx = type_idx,
            .scope_level = static_cast<uint16_t>(scope),
            .flags = 0
        };
        symbols.emplace_back(sym);
        ast.strings.emplace_back(name);
    }

    bool Parser::symbol_exists(std::string_view name) const
    {
        return std::ranges::any_of(symbols,
                                   [&](const Symbol &s)
                                   {
                                       return ast.strings[s.name_index] == name && s.scope_level == scope;
                                   });
    }

    void Parser::enter_scope()
    {
        scope_boundaries.emplace_back(symbols.size());
        scope++;
    }

    void Parser::exit_scope()
    {
        const auto boundary = scope_boundaries.back();
        symbols.erase(symbols.begin() + boundary, symbols.end());
        scope_boundaries.pop_back();
        scope--;
    }

    Symbol *Parser::get_symbol(std::string_view name)
    {
        for (auto &symbol: std::ranges::reverse_view(symbols))
        {
            if (ast.strings[symbol.name_index] == name)
                return &symbol;
        }
        return nullptr;
    }

    std::pair<uint32_t, uint32_t> Parser::get_line_col(const Token &token)
    {
        const auto it = std::ranges::upper_bound(line_starts, token.start);
        return { std::distance(line_starts.begin(), it), token.start - *(it - 1) + 1 };
    }

    bool Parser::is_at_end() const
    {
        return current_pos >= tokens.size() || tokens.types[current_pos] == TokenType::END_OF_FILE;
    }
}
