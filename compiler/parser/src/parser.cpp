#include <compiler/parser/include/parser.h>

namespace klr::compiler
{
    Parser::Parser(TokenList &tokens) : tokens(tokens) {}

    AST Parser::parse()
    {
        constexpr Token root_token {};
        const uint32_t root = ast.add_node(ASTNodeType::ROOT, root_token);
        while (!is_at_end())
        {
            switch (peek().type)
            {
                case TokenType::VAR:
                case TokenType::CONST:
                {
                    const uint32_t decl = parse_decl();
                    ast.add_child(root, decl);
                    break;
                }
                case TokenType::FUNCTION:
                {
                    const uint32_t func = parse_function(false);
                    ast.add_child(root, func);
                    break;
                }
                /* more cases TBA */
                default:
                {
                    advance();
                }
            }
        }

        return ast;
    }

    Token Parser::peek() const
    {
        return tokens[current];
    }

    Token Parser::advance()
    {
        return tokens[current++];
    }

    bool Parser::is_at_end() const
    {
        return current >= tokens.size();
    }

    bool Parser::match(TokenType type)
    {
        if (is_at_end())
            return false;
        if (peek().type != type)
            return false;

        advance();
        return true;
    }

    Token Parser::expect(const TokenType type)
    {
        if (is_at_end())
            return {};
        if (peek().type != type)
            return {};

        return advance();
    }

    uint32_t Parser::parse_decl() // NOLINT(*-no-recursion)
    {
        auto flags = ASTNodeFlags::NONE;
        if (peek().type == TokenType::CONST)
            flags = flags | ASTNodeFlags::IS_CONST;

        advance();
        const Token name = expect(TokenType::IDENTIFIER);

        const uint32_t decl_node = ast.add_node(ASTNodeType::DECL, name);
        ast.nodes[decl_node].data.decl.flags = flags;

        if (const Token colon = expect(TokenType::COLON);
            colon.type != TokenType::COLON)
        {
            flags = flags | ASTNodeFlags::TYPE_INFER;
            ast.nodes[decl_node].data.decl.flags = flags;
            ast.nodes[decl_node].data.decl.type_node = 0;
        }
        else
        {
            const uint32_t type_idx = parse_type();
            ast.nodes[decl_node].data.decl.type_node = type_idx;
            ast.add_child(decl_node, type_idx);
        }

        if (match(TokenType::EQUAL))
        {
            const uint32_t init_idx = parse_expression();
            ast.nodes[decl_node].data.decl.init_node = init_idx;
            ast.add_child(decl_node, init_idx);
        }
        else
        {
            ast.nodes[decl_node].data.decl.init_node = 0;
        }

        expect(TokenType::SEMICOLON);
        return decl_node;
    }

    // ReSharper disable once CppDFAConstantParameter
    uint32_t Parser::parse_function(const bool is_lambda) // NOLINT(*-no-recursion)
    {
        const Token func_token = advance();
        const uint32_t func = ast.add_node(ASTNodeType::FUNCTION, func_token);

        /* for regular functions (not lambdas) parse name*/
        if (!is_lambda)
        {
            expect(TokenType::IDENTIFIER);
        }

        if (match(TokenType::LESS))
        {
            do
            {
                const Token param = expect(TokenType::IDENTIFIER);
                const uint32_t generic_param = ast.add_node(ASTNodeType::TYPE, param);
                if (match(TokenType::DOT))
                {
                    expect(TokenType::DOT);
                    expect(TokenType::DOT);
                    // TODO: add variadic flag to generic_param
                }
                ast.add_child(func, generic_param);
            }
            while (match(TokenType::COMMA));
            expect(TokenType::GREATER);
        }

        /* parameter list */
        /* parameter list */
        expect(TokenType::LEFT_PAREN);
        while (peek().type != TokenType::RIGHT_PAREN)
        {
            const Token param_name = expect(TokenType::IDENTIFIER);
            if (param_name.type != TokenType::IDENTIFIER)
                break;

            if (expect(TokenType::COLON).type != TokenType::COLON)
                break;

            const uint32_t param_type = parse_type();
            if (param_type == 0)
                break;

            const uint32_t param_node = ast.add_node(ASTNodeType::DECL, param_name);
            ast.nodes[param_node].data.decl.type_node = param_type;
            ast.add_child(param_node, param_type);
            ast.add_child(func, param_node);
            if (peek().type != TokenType::COMMA && peek().type != TokenType::RIGHT_PAREN)
                break; /* TODO: throw error & break! */

            if (!match(TokenType::COMMA))
                break; /* end of parameters */
        }
        expect(TokenType::RIGHT_PAREN);

        /* return type */
        expect(TokenType::ARROW);
        const uint32_t return_type = parse_type();
        ast.nodes[func].data.function.ret_type = return_type;
        ast.add_child(func, return_type);

        const Token brace = expect(TokenType::LEFT_BRACE);
        const uint32_t body = parse_block(brace);
        ast.nodes[func].data.function.body = body;
        ast.add_child(func, body);

        return func;
    }

    uint32_t Parser::parse_type() // NOLINT(*-no-recursion)
    {
        const Token type_tk = peek();
        uint32_t type_index = 0;

        switch (type_tk.type)
        {
            /* builtin primitives */
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
                advance();
                type_index = ast.add_node(ASTNodeType::TYPE, type_tk);
                break;
            }

            /* builtin pointers */
            case TokenType::OWN:
            case TokenType::SHARE:
            case TokenType::REF:
            case TokenType::PIN:
            {
                advance();
                expect(TokenType::LESS);
                type_index = ast.add_node(ASTNodeType::TYPE, type_tk);
                const uint32_t inner_type = parse_type();
                ast.add_child(type_index, inner_type);
                expect(TokenType::GREATER);
                break;
            }

            /* user defined & generics */
            case TokenType::IDENTIFIER:
            {
                advance();
                type_index = ast.add_node(ASTNodeType::TYPE, type_tk);
                break;
            }

            /* unexpected token! */
            default:
                break; /* TODO: error handling */
        }

        /* array types */
        if (match(TokenType::LEFT_BRACKET))
        {
            expect(TokenType::RIGHT_BRACKET);
            const uint32_t arr_type = ast.add_node(ASTNodeType::ARRAY_TYPE, type_tk);
            ast.add_child(arr_type, type_index);
            return arr_type;
        }

        return type_index;
    }

    uint32_t Parser::parse_primary() // NOLINT(*-no-recursion)
    {
        switch (const Token tk = peek(); tk.type)
        {
            case TokenType::STRING:
            case TokenType::NUM_LITERAL:
            case TokenType::TRUE:
            case TokenType::FALSE:
            case TokenType::NIL:
            {
                advance();
                return ast.add_node(ASTNodeType::LITERAL, tk);
            }
            case TokenType::LEFT_BRACE:
            {
                advance();
                const uint32_t arr_init = ast.add_node(ASTNodeType::ARRAY_INIT, tk);
                while (!match(TokenType::RIGHT_BRACE))
                {
                    ast.add_child(arr_init, parse_expression());
                    if (!match(TokenType::COMMA))
                        break;
                }
                expect(TokenType::RIGHT_BRACE);
                return arr_init;
            }
            case TokenType::LEFT_PAREN:
            {
                advance();
                const uint32_t expr = parse_expression();
                expect(TokenType::RIGHT_PAREN);
                return expr;
            }
            case TokenType::IDENTIFIER:
            {
                advance();
                uint32_t id = ast.add_node(ASTNodeType::IDENTIFIER, tk);

                while (true)
                {
                    if (match(TokenType::DOT))
                    {
                        const Token method = expect(TokenType::IDENTIFIER);
                        if (!match(TokenType::LEFT_PAREN))
                            return 0; // TODO: error handling

                        const uint32_t call = ast.add_node(ASTNodeType::METHOD_CALL, method);
                        ast.add_child(call, id);

                        while (!match(TokenType::RIGHT_PAREN))
                        {
                            ast.add_child(call, parse_expression());
                            if (!match(TokenType::COMMA))
                                break;
                        }
                        id = call;
                    }
                    else if (match(TokenType::LEFT_PAREN))
                    {
                        const uint32_t call = ast.add_node(ASTNodeType::CALL, tk);
                        ast.add_child(call, id);

                        while (!match(TokenType::RIGHT_PAREN))
                        {
                            ast.add_child(call, parse_expression());
                            if (!match(TokenType::COMMA))
                                break;
                        }
                        id = call;
                    }
                    else
                    {
                        break;
                    }
                }
                return id;
            }
            case TokenType::CAST:
            {
                advance();
                expect(TokenType::LESS);
                const uint32_t cast_type = parse_type();
                expect(TokenType::GREATER);
                expect(TokenType::LEFT_PAREN);
                const uint32_t expr = parse_expression();
                expect(TokenType::RIGHT_PAREN);

                const uint32_t cast = ast.add_node(ASTNodeType::CAST_EXPR, tk);
                ast.nodes[cast].data.cast_expr = {
                    .operand = expr,
                    .type_node = cast_type
                };
                return cast;
            }
            case TokenType::FUNCTION:
            {
                return parse_function(true);
            }

            default:
                return 0; // TODO: error handling
        }
    }

    uint32_t Parser::parse_expression() // NOLINT(*-no-recursion)
    {
        return parse_assignment();
    }

    uint32_t Parser::parse_assignment() // NOLINT(*-no-recursion)
    {
        const uint32_t expr = parse_ternary();

        Token op_token {};
        auto is_assignment = false;
        if (peek().type == TokenType::EQUAL ||
            peek().type == TokenType::PLUS_EQ ||
            peek().type == TokenType::MINUS_EQ ||
            peek().type == TokenType::STAR_EQ ||
            peek().type == TokenType::SLASH_EQ ||
            peek().type == TokenType::PERCENT_EQ ||
            peek().type == TokenType::AND_EQ ||
            peek().type == TokenType::OR_EQ ||
            peek().type == TokenType::XOR_EQ ||
            peek().type == TokenType::LEFT_SHIFT_EQ ||
            peek().type == TokenType::RIGHT_SHIFT_EQ)
        {
            op_token = advance();
            is_assignment = true;
        }

        if (is_assignment)
        {
            const uint32_t value = parse_expression();
            const uint32_t assign = ast.add_node(ASTNodeType::BINARY_EXPR, op_token);
            ast.nodes[assign].data.binary_expr = {
                .left = expr,
                .right = value,
                .op = op_token.type
            };
            return assign;
        }

        return expr;
    }

    uint32_t Parser::parse_ternary() // NOLINT(*-no-recursion)
    {
        const uint32_t expr = parse_logical_or();
        if (peek().type == TokenType::QUESTION)
        {
            const Token question = advance();
            const uint32_t then_branch = parse_expression();
            expect(TokenType::COLON);
            const uint32_t else_branch = parse_expression();

            const uint32_t ternary = ast.add_node(ASTNodeType::TERNARY, question);
            ast.add_child(ternary, expr);
            ast.add_child(ternary, then_branch);
            ast.add_child(ternary, else_branch);
            return ternary;
        }

        return expr;
    }

    uint32_t Parser::parse_logical_or() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_logical_and();
        while (peek().type == TokenType::LOGICAL_OR)
        {
            const Token op = advance();
            const uint32_t right = parse_logical_and();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_logical_and() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_bitwise_or();
        while (peek().type == TokenType::LOGICAL_AND)
        {
            const Token op = advance();
            const uint32_t right = parse_bitwise_or();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_bitwise_or() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_bitwise_xor();
        while (peek().type == TokenType::OR)
        {
            const Token op = advance();
            const uint32_t right = parse_bitwise_xor();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_bitwise_xor() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_bitwise_and();
        while (peek().type == TokenType::XOR)
        {
            const Token op = advance();
            const uint32_t right = parse_bitwise_and();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_bitwise_and() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_shift();
        while (peek().type == TokenType::AND)
        {
            const Token op = advance();
            const uint32_t right = parse_shift();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_shift() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_equality();
        while (peek().type == TokenType::LEFT_SHIFT || peek().type == TokenType::RIGHT_SHIFT)
        {
            const Token op = advance();
            const uint32_t right = parse_equality();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_equality() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_comparison();
        while (peek().type == TokenType::EQ || peek().type == TokenType::NE)
        {
            const Token op = advance();
            const uint32_t right = parse_comparison();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_comparison() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_term();
        while (peek().type == TokenType::LESS || peek().type == TokenType::LE ||
               peek().type == TokenType::GREATER || peek().type == TokenType::GE)
        {
            const Token op = advance();
            const uint32_t right = parse_term();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_term() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_factor();
        while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS)
        {
            const Token op = advance();
            const uint32_t right = parse_factor();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_factor() // NOLINT(*-no-recursion)
    {
        uint32_t expr = parse_unary();
        while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH ||
               peek().type == TokenType::PERCENT)
        {
            const Token op = advance();
            const uint32_t right = parse_unary();
            const uint32_t binary = ast.add_node(ASTNodeType::BINARY_EXPR, op);
            ast.nodes[binary].data.binary_expr = {
                .left = expr,
                .right = right,
                .op = op.type
            };
            expr = binary;
        }

        return expr;
    }

    uint32_t Parser::parse_unary() // NOLINT(*-no-recursion)
    {
        if (peek().type == TokenType::BANG ||
            peek().type == TokenType::MINUS ||
            peek().type == TokenType::TILDE ||
            peek().type == TokenType::NEW ||
            peek().type == TokenType::DELETE ||
            peek().type == TokenType::AND || /* ref op */
            peek().type == TokenType::STAR) /* ptr deref op */
        {
            const Token op = advance();
            const uint32_t right = parse_unary();
            const uint32_t unary = ast.add_node(ASTNodeType::UNARY_EXPR, op);
            ast.nodes[unary].data.unary_expr = {
                .operand = right,
                .op = op.type
            };
            return unary;
        }

        return parse_primary();
    }

    uint32_t Parser::parse_if() // NOLINT(*-no-recursion)
    {
        const Token if_token = advance(); // Save IF token
        expect(TokenType::LEFT_PAREN);
        const uint32_t condition = parse_expression();
        expect(TokenType::RIGHT_PAREN);

        const Token then_brace = expect(TokenType::LEFT_BRACE);
        const uint32_t then_branch = parse_block(then_brace);
        uint32_t else_branch = 0;

        if (match(TokenType::ELSE))
        {
            if (match(TokenType::IF))
            {
                else_branch = parse_if();
            }
            else
            {
                const Token else_brace = expect(TokenType::LEFT_BRACE);
                else_branch = parse_block(else_brace);
            }
        }

        const uint32_t if_node = ast.add_node(ASTNodeType::IF, if_token);
        ast.add_child(if_node, condition);
        ast.add_child(if_node, then_branch);
        if (else_branch)
            ast.add_child(if_node, else_branch);

        return if_node;
    }

    uint32_t Parser::parse_while() // NOLINT(*-no-recursion)
    {
        const Token while_token = advance();
        expect(TokenType::LEFT_PAREN);
        const uint32_t condition = parse_expression();
        expect(TokenType::RIGHT_PAREN);

        const Token brace = expect(TokenType::LEFT_BRACE);
        const uint32_t body = parse_block(brace);

        const uint32_t while_node = ast.add_node(ASTNodeType::WHILE, while_token);
        ast.add_child(while_node, condition);
        ast.add_child(while_node, body);

        return while_node;
    }

    uint32_t Parser::parse_for() // NOLINT(*-no-recursion)
    {
        const Token for_token = advance();
        expect(TokenType::LEFT_PAREN);

        uint32_t init = 0;
        if (!match(TokenType::SEMICOLON))
        {
            init = match(TokenType::VAR) || match(TokenType::CONST)
                       ? parse_decl()
                       : parse_expression();
            expect(TokenType::SEMICOLON);
        }

        uint32_t condition = 0;
        if (!match(TokenType::SEMICOLON))
        {
            condition = parse_expression();
            expect(TokenType::SEMICOLON);
        }

        uint32_t increment = 0;
        if (!match(TokenType::RIGHT_PAREN))
        {
            increment = parse_expression();
            expect(TokenType::RIGHT_PAREN);
        }

        const Token brace = expect(TokenType::LEFT_BRACE);
        const uint32_t body = parse_block(brace);
        const uint32_t for_node = ast.add_node(ASTNodeType::FOR, for_token);

        if (init)
            ast.add_child(for_node, init);
        if (condition)
            ast.add_child(for_node, condition);
        if (increment)
            ast.add_child(for_node, increment);
        ast.add_child(for_node, body);

        return for_node;
    }

    uint32_t Parser::parse_block(const Token brace) // NOLINT(*-no-recursion)
    {
        const uint32_t block = ast.add_node(ASTNodeType::BLOCK, brace);
        while (!match(TokenType::RIGHT_BRACE))
        {
            switch (peek().type)
            {
                case TokenType::VAR:
                case TokenType::CONST:
                {
                    ast.add_child(block, parse_decl());
                    break;
                }

                case TokenType::RETURN:
                {
                    const Token return_token = advance();
                    const uint32_t ret = ast.add_node(ASTNodeType::RETURN, return_token);
                    if (!match(TokenType::SEMICOLON))
                    {
                        ast.add_child(ret, parse_expression());
                        expect(TokenType::SEMICOLON);
                    }
                    ast.add_child(block, ret);
                    break;
                }

                case TokenType::IF:
                {
                    ast.add_child(block, parse_if());
                    break;
                }

                case TokenType::WHILE:
                {
                    ast.add_child(block, parse_while());
                    break;
                }

                case TokenType::FOR:
                {
                    ast.add_child(block, parse_for());
                    break;
                }

                case TokenType::BREAK:
                {
                    Token break_token = advance();
                    ast.add_child(block, ast.add_node(ASTNodeType::BREAK, break_token));
                    expect(TokenType::SEMICOLON);
                    break;
                }

                case TokenType::CONTINUE:
                {
                    Token continue_token = advance();
                    ast.add_child(block, ast.add_node(ASTNodeType::CONTINUE, continue_token));
                    expect(TokenType::SEMICOLON);
                    break;
                }

                default:
                {
                    const uint32_t expr = parse_expression();
                    expect(TokenType::SEMICOLON);
                    ast.add_child(block, expr);
                    break;
                }
            }
        }

        return block;
    }
}
