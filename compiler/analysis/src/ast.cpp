#include "../include/ast.h"

namespace klr::compiler
{
    uint32_t AST::add_node(const ASTNodeType type, const Token token)
    {
        const uint32_t index = nodes.size();
        nodes.emplace_back(ASTNode {
            .children = {},
            .token = token,
            .parent = 0,
            .type = type,
            .data = {}
        });
        return index;
    }

    void AST::add_child(uint32_t parent_idx, uint32_t child_idx)
    {
        if (parent_idx >= nodes.size() || child_idx >= nodes.size())
            return;

        nodes[parent_idx].children.push_back(child_idx);
        nodes[child_idx].parent = parent_idx;
    }

    void AST::dump(std::ostream &os, const uint32_t node_idx, const size_t indent) const
    {
        std::vector visited(nodes.size(), false);
        dump_node(os, node_idx, indent, visited);
    }

    void AST::dump_node(std::ostream &os, const uint32_t node_idx, const size_t indent, // NOLINT(*-no-recursion)
                        std::vector<bool> &visited) const
    {
        if (node_idx >= nodes.size() || visited[node_idx])
            return;

        visited[node_idx] = true;
        const auto &node = nodes[node_idx];
        std::string indent_str;

        if (indent > 0)
        {
            for (size_t i = 0; i < indent - 1; i++)
                indent_str += "│   ";
            indent_str += "├── ";
        }

        os << indent_str
                << ColorCode::BOLD << ColorCode::BLUE << "Node " << node_idx << ColorCode::RESET
                << " [" << ColorCode::GREEN << node_type_to_string(node.type) << ColorCode::RESET << "]";

        os << " " << ColorCode::YELLOW
                << "token{start=" << node.token.start
                << " len=" << node.token.len
                << " type=" << token_to_str(node.token.type) << "}"
                << ColorCode::RESET;

        if (node.token.flags != TokenFlags::NONE)
        {
            os << " " << ColorCode::MAGENTA << "flags=";
            dump_token_flags(os, node.token.flags);
            os << ColorCode::RESET;
        }

        os << "\n";
        std::string data_indent;
        for (size_t i = 0; i < indent; i++)
            data_indent += "│   ";

        switch (node.type)
        {
            case ASTNodeType::DECL:
            {
                os << data_indent << ColorCode::CYAN << "└─ flags: ";
                dump_node_flags(os, node.data.decl.flags);
                os << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ init_node: " << node.data.decl.init_node << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ type_node: " << node.data.decl.type_node << ColorCode::RESET << "\n";
                break;
            }
            case ASTNodeType::BINARY_EXPR:
            {
                os << data_indent << ColorCode::CYAN
                        << "└─ left: " << node.data.binary_expr.left << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ right: " << node.data.binary_expr.right << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ operator: " << token_to_str(node.data.binary_expr.op) << ColorCode::RESET << "\n";
                break;
            }
            case ASTNodeType::UNARY_EXPR:
            {
                os << data_indent << ColorCode::CYAN
                        << "└─ operand: " << node.data.unary_expr.operand << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ operator: " << token_to_str(node.data.unary_expr.op) << ColorCode::RESET << "\n";
                break;
            }
            case ASTNodeType::FUNCTION:
            {
                os << data_indent << ColorCode::CYAN
                        << "└─ ret_type: " << node.data.function.ret_type << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ body: " << node.data.function.body << ColorCode::RESET << "\n";
                break;
            }
            case ASTNodeType::CAST_EXPR:
            {
                os << data_indent << ColorCode::CYAN
                        << "└─ operand: " << node.data.cast_expr.operand << ColorCode::RESET << "\n";
                os << data_indent << ColorCode::CYAN
                        << "└─ type_node: " << node.data.cast_expr.type_node << ColorCode::RESET << "\n";
                break;
            }
            default:
                break;
        }

        os << data_indent << ColorCode::RED
                << "└─ parent: " << node.parent << ColorCode::RESET << "\n";

        if (!node.children.empty())
        {
            os << data_indent << ColorCode::MAGENTA
                    << "└─ children: [";
            for (size_t i = 0; i < node.children.size(); ++i)
            {
                os << node.children[i];
                if (i < node.children.size() - 1)
                    os << ", ";
            }
            os << "]" << ColorCode::RESET << "\n";
        }

        switch (node.type)
        {
            case ASTNodeType::BINARY_EXPR:
            {
                dump_node(os, node.data.binary_expr.left, indent + 1, visited);
                dump_node(os, node.data.binary_expr.right, indent + 1, visited);
                break;
            }
            case ASTNodeType::UNARY_EXPR:
            {
                dump_node(os, node.data.unary_expr.operand, indent + 1, visited);
                break;
            }
            case ASTNodeType::DECL:
            {
                if (node.data.decl.type_node)
                    dump_node(os, node.data.decl.type_node, indent + 1, visited);
                if (node.data.decl.init_node)
                    dump_node(os, node.data.decl.init_node, indent + 1, visited);
                break;
            }
            case ASTNodeType::FUNCTION:
            {
                dump_node(os, node.data.function.ret_type, indent + 1, visited);
                dump_node(os, node.data.function.body, indent + 1, visited);
                break;
            }
            case ASTNodeType::CAST_EXPR:
            {
                dump_node(os, node.data.cast_expr.type_node, indent + 1, visited);
                dump_node(os, node.data.cast_expr.operand, indent + 1, visited);
                break;
            }
            default:
                break;
        }

        for (const auto &child: node.children)
            dump_node(os, child, indent + 1, visited);
    }

    void AST::dump_token_flags(std::ostream &os, const TokenFlags flags)
    {
        os << static_cast<uint8_t>(flags);
    }

    void AST::dump_node_flags(std::ostream &os, const ASTNodeFlags flags)
    {
        if (flags == ASTNodeFlags::NONE)
        {
            os << "NONE";
            return;
        }

        auto first = true;
        if ((flags & ASTNodeFlags::IS_CONST) != ASTNodeFlags::NONE)
        {
            os << "IS_CONST";
            first = false;
        }
        if ((flags & ASTNodeFlags::TYPE_INFER) != ASTNodeFlags::NONE)
        {
            if (!first)
                os << " | ";
            os << "TYPE_INFER";
        }
    }

    const char *AST::node_type_to_string(ASTNodeType type)
    {
        switch (type)
        {
            case ASTNodeType::ROOT:
                return "ROOT";
            case ASTNodeType::DECL:
                return "DECL";
            case ASTNodeType::FUNCTION:
                return "FUNCTION";
            case ASTNodeType::ARRAY_TYPE:
                return "ARRAY_TYPE";
            case ASTNodeType::TYPE:
                return "TYPE";
            case ASTNodeType::BINARY_EXPR:
                return "BINARY_EXPR";
            case ASTNodeType::UNARY_EXPR:
                return "UNARY_EXPR";
            case ASTNodeType::LITERAL:
                return "LITERAL";
            case ASTNodeType::IDENTIFIER:
                return "IDENTIFIER";
            case ASTNodeType::ARRAY_INIT:
                return "ARRAY_INIT";
            case ASTNodeType::METHOD_CALL:
                return "METHOD_CALL";
            case ASTNodeType::CALL:
                return "CALL";
            case ASTNodeType::TERNARY:
                return "TERNARY";
            case ASTNodeType::BLOCK:
                return "BLOCK";
            case ASTNodeType::RETURN:
                return "RETURN";
            case ASTNodeType::IF:
                return "IF";
            case ASTNodeType::WHILE:
                return "WHILE";
            case ASTNodeType::FOR:
                return "FOR";
            case ASTNodeType::BREAK:
                return "BREAK";
            case ASTNodeType::CONTINUE:
                return "CONTINUE";
            case ASTNodeType::CAST_EXPR:
                return "CAST_EXPR";
            default:
                return "UNKNOWN";
        }
    }
}
