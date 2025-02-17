#include <compiler/parser/include/parser.h>
#include <sstream>

namespace klr::compiler
{
    Parser::Parser(const std::string_view module_name, const std::string_view source, TokenList &tokens,
                   const std::unique_ptr<std::vector<uint32_t> > &starts) : line_starts(std::move(*starts)),
                                                                            mod_name(module_name), src(source),
                                                                            tokens(tokens) {}

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
                case TokenType::CLASS:
                case TokenType::STRUCT:
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

    bool Parser::match(const TokenType type)
    {
        if (is_at_end())
            return false;
        if (peek().type != type)
            return false;

        advance();
        return true;
    }

    Token Parser::expect(const TokenType type, const bool err)
    {
        if (const auto ne = peek();
            is_at_end() || ne.type != type)
        {
            if (err)
                error(ne, "unexpected token");
            return {};
        }

        return advance();
    }

    Token Parser::expect(const TokenType type, const char* str)
    {
        if (const auto ne = peek();
            is_at_end() || ne.type != type)
        {
            error(ne, str);
        }

        return advance();
    }

    std::pair<uint32_t, uint32_t> Parser::get_position(const uint32_t offset) const
    {
        const auto line_it = std::ranges::upper_bound(line_starts, offset);
        size_t line = std::distance(line_starts.begin(), line_it) - 1;
        uint32_t column = offset - line_starts[line];
        return { line, column };
    }

    void Parser::error(const Token token, std::string_view message, std::string_view help) const
    {
        auto [line, column] = get_position(token.start);

        const uint32_t display_line = line + 1;
        const uint32_t display_column = column;

        const std::string RED = "\033[31m";
        const std::string BRIGHT_RED = "\033[91m";
        const std::string RESET = "\033[0m";

        std::stringstream error_msg;
        error_msg << RED << mod_name << ":" << display_line << ":" << display_column << RESET
                  << " " << BRIGHT_RED << "\n" << "error" << ": " << message << "\n";

        const std::string_view pline = line_starts.size() > 1 ? src.substr(
                                               line_starts[line],
                                               line_starts[line + 1] - line_starts[line]) : src;

        error_msg << "\n";
        error_msg << " " << RED << display_line << " | " << BRIGHT_RED << pline << RESET << "\n";
        error_msg << std::string(std::to_string(display_line).length() + 2 + display_column - 1, ' ');
        error_msg << BRIGHT_RED << "^" << "\n";

        if (!help.empty())
            error_msg << RESET << RED << help << "\n";

        throw std::runtime_error(error_msg.str());
    }

    uint32_t Parser::parse_decl() // NOLINT(*-no-recursion)
    {
        auto flags = ASTNodeFlags::NONE;
        if (peek().type == TokenType::CONST)
            flags = flags | ASTNodeFlags::IS_CONST;

        advance();
        const Token name = expect(TokenType::IDENTIFIER, "is this a valid name?");

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

        if (const Token eq = expect(TokenType::EQUAL, true);
            eq.type == TokenType::EQUAL)
        {
            const uint32_t init_idx = parse_expression();
            ast.nodes[decl_node].data.decl.init_node = init_idx;
            ast.add_child(decl_node, init_idx);
        }
        else
        {
            ast.nodes[decl_node].data.decl.init_node = 0;
        }

        expect(TokenType::SEMICOLON, true);
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
            expect(TokenType::IDENTIFIER, "is this a valid name?");
        }

        if (match(TokenType::LESS))
        {
            do
            {
                const Token param = expect(TokenType::IDENTIFIER, true);
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
            expect(TokenType::GREATER, true);
        }

        /* parameter list */
        /* parameter list */
        expect(TokenType::LEFT_PAREN, true);
        while (peek().type != TokenType::RIGHT_PAREN)
        {
            const Token param_name = expect(TokenType::IDENTIFIER, "invalid parameter");
            if (param_name.type != TokenType::IDENTIFIER)
                break;

            if (expect(TokenType::COLON, true).type != TokenType::COLON)
                break;

            const uint32_t param_type = parse_type();
            if (param_type == 0)
                break;

            const uint32_t param_node = ast.add_node(ASTNodeType::DECL, param_name);
            ast.nodes[param_node].data.decl.type_node = param_type;
            ast.add_child(param_node, param_type);
            ast.add_child(func, param_node);
            if (peek().type != TokenType::COMMA && peek().type != TokenType::RIGHT_PAREN)
            {
                error(peek(),
                      "invalid parameter list",
                      "parameters must be separated by commas");
            }

            if (!match(TokenType::COMMA))
                break; /* end of parameters */
        }
        expect(TokenType::RIGHT_PAREN, true);

        /* return type */
        expect(TokenType::ARROW, true);
        const uint32_t return_type = parse_type();
        ast.nodes[func].data.function.ret_type = return_type;
        ast.add_child(func, return_type);

        const Token brace = expect(TokenType::LEFT_BRACE, true);
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
                expect(TokenType::LESS, true);
                type_index = ast.add_node(ASTNodeType::TYPE, type_tk);
                const uint32_t inner_type = parse_type();
                ast.add_child(type_index, inner_type);
                expect(TokenType::GREATER, true);
                break;
            }

            /* user defined & generics */
            case TokenType::IDENTIFIER:
            {
                advance();
                type_index = ast.add_node(ASTNodeType::TYPE, type_tk);

                /* if this is a generic identifier type */
                if (match(TokenType::LESS))
                {
                    while (!match(TokenType::GREATER))
                    {
                        const uint32_t param_type = parse_type();
                        ast.add_child(type_index, param_type);

                        if (!match(TokenType::COMMA))
                            break;
                    }
                }
                break;
            }

            /* unexpected token! */
            default:
            {
                error(peek(),
                      "unexpected token in type parsing",
                      "expected a valid type");
            }
        }

        /* array types */
        if (match(TokenType::LEFT_BRACKET))
        {
            expect(TokenType::RIGHT_BRACKET, true);
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
                expect(TokenType::RIGHT_BRACE, true);
                return arr_init;
            }
            case TokenType::LEFT_PAREN:
            {
                advance();
                const uint32_t expr = parse_expression();
                expect(TokenType::RIGHT_PAREN, true);
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
                        const Token method = expect(TokenType::IDENTIFIER, true);
                        if (!match(TokenType::LEFT_PAREN))
                        {
                            error(peek(),
                                  "invalid method call",
                                  "method calls must be followed by parentheses");
                        }

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
                expect(TokenType::LESS, true);
                const uint32_t cast_type = parse_type();
                expect(TokenType::GREATER, true);
                expect(TokenType::LEFT_PAREN, true);
                const uint32_t expr = parse_expression();
                expect(TokenType::RIGHT_PAREN, true);

                const uint32_t cast = ast.add_node(ASTNodeType::CAST_EXPR, tk);
                ast.nodes[cast].data.cast_expr = {
                    .operand = expr,
                    .type_node = cast_type
                };

                ast.add_child(cast, cast_type);
                ast.add_child(cast, expr);
                return cast;
            }
            case TokenType::FUNCTION:
            {
                return parse_function(true);
            }

            default:
            {
                error(peek(),
                      "unexpected token in primary expression",
                      "unable to parse this token as a primary expression");
            }
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
            expect(TokenType::COLON, true);
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
            peek().type == TokenType::AND || /* ref op */
            peek().type == TokenType::STAR)  /* ptr deref op */
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
        if (peek().type == TokenType::NEW)
        {
            const Token op = advance();
            const uint32_t type_node = parse_type();

            const uint32_t unary = ast.add_node(ASTNodeType::UNARY_EXPR, op);
            if (peek().type == TokenType::LEFT_BRACE || peek().type == TokenType::LEFT_PAREN)
            {
                const uint32_t init = parse_primary();
                ast.nodes[unary].data.unary_expr = {
                    .operand = init,
                    .op = op.type
                };
                ast.add_child(unary, type_node);
                ast.add_child(unary, init);
            }
            else
            {
                ast.nodes[unary].data.unary_expr = {
                    .operand = type_node,
                    .op = op.type
                };
                ast.add_child(unary, type_node);
            }
            return unary;
        }
        if (peek().type == TokenType::DELETE)
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
        expect(TokenType::LEFT_PAREN, true);
        const uint32_t condition = parse_expression();
        expect(TokenType::RIGHT_PAREN, true);

        const Token then_brace = expect(TokenType::LEFT_BRACE, true);
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
                const Token else_brace = expect(TokenType::LEFT_BRACE, true);
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
        expect(TokenType::LEFT_PAREN, true);
        const uint32_t condition = parse_expression();
        expect(TokenType::RIGHT_PAREN, true);

        const Token brace = expect(TokenType::LEFT_BRACE, true);
        const uint32_t body = parse_block(brace);

        const uint32_t while_node = ast.add_node(ASTNodeType::WHILE, while_token);
        ast.add_child(while_node, condition);
        ast.add_child(while_node, body);

        return while_node;
    }

    uint32_t Parser::parse_for() // NOLINT(*-no-recursion)
    {
        const Token for_token = advance();
        expect(TokenType::LEFT_PAREN, true);

        uint32_t init = 0;
        if (!match(TokenType::SEMICOLON))
        {
            init = match(TokenType::VAR) || match(TokenType::CONST)
                       ? parse_decl()
                       : parse_expression();
            expect(TokenType::SEMICOLON, true);
        }

        uint32_t condition = 0;
        if (!match(TokenType::SEMICOLON))
        {
            condition = parse_expression();
            expect(TokenType::SEMICOLON, true);
        }

        uint32_t increment = 0;
        if (!match(TokenType::RIGHT_PAREN))
        {
            increment = parse_expression();
            expect(TokenType::RIGHT_PAREN, true);
        }

        const Token brace = expect(TokenType::LEFT_BRACE, true);
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
                        expect(TokenType::SEMICOLON, true);
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
                    const Token break_token = advance();
                    ast.add_child(block, ast.add_node(ASTNodeType::BREAK, break_token));
                    expect(TokenType::SEMICOLON, true);
                    break;
                }

                case TokenType::CONTINUE:
                {
                    const Token continue_token = advance();
                    ast.add_child(block, ast.add_node(ASTNodeType::CONTINUE, continue_token));
                    expect(TokenType::SEMICOLON, true);
                    break;
                }

                default:
                {
                    const uint32_t expr = parse_expression();
                    expect(TokenType::SEMICOLON, true);
                    ast.add_child(block, expr);
                    break;
                }
            }
        }

        return block;
    }
}
