#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <ostream>
#include <compiler/interfaces/include/tokens.h>

namespace klr::compiler
{
    enum class ASTNodeType : uint8_t
    {
        ROOT,
        DECL,        /* var/const declarations */
        FUNCTION,    /* function definitions */
        ARRAY_TYPE,  /* array type */
        TYPE,        /* type annotations */
        BINARY_EXPR, /* binary expressions */
        UNARY_EXPR,  /* unary expressions */
        LITERAL,     /* literals (check tokens to determine if it's a string or number) */
        IDENTIFIER,  /* variable references */
        ARRAY_INIT,  /* array declaration */
        METHOD_CALL, /* method call */
        CALL,        /* function call */
        TERNARY,     /* shorthand call */
        BLOCK,       /* for function bodies and control flow blocks */
        RETURN,      /* return statements */
        IF,
        WHILE,
        FOR,
        BREAK,
        CONTINUE,
    };

    enum class ASTNodeFlags : uint8_t
    {
        NONE = 0,
        IS_CONST = 1 << 0,   /* for const declarations */
        TYPE_INFER = 1 << 1, /* if this flag is present, the type engine needs to infer */
    };

    inline ASTNodeFlags operator|(ASTNodeFlags a, ASTNodeFlags b)
    {
        return static_cast<ASTNodeFlags>(
            static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
        );
    }

    inline ASTNodeFlags operator&(ASTNodeFlags lhs, ASTNodeFlags rhs)
    {
        return static_cast<ASTNodeFlags>(
            static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
        );
    }

    struct ASTNode
    {
        std::vector<uint32_t> children; /* child indices */
        Token token;                    /* the token (metadata is inside) */
        uint32_t parent;                /* parent index */
        ASTNodeType type;

        union
        {
            struct
            {
                ASTNodeFlags flags;
                uint32_t init_node; /* index to initialization expression */
                uint32_t type_node; /* index to type expression */
            } decl;

            struct
            {
                uint32_t left;  /* index to left operand */
                uint32_t right; /* index to right operand */
                TokenType op;   /* operator type (e.g., PLUS, MINUS) */
            } binary_expr;

            struct
            {
                uint32_t operand; /* index to the operand */
                TokenType op;     /* operator type (e.g., BANG, MINUS) */
            } unary_expr;

            struct
            {
                uint32_t ret_type; /* index to return type node */
                uint32_t body;     /* index to function body */
            } function;
        } data;
    };

    class AST
    {
    public:
        std::vector<ASTNode> nodes;

        /**
         * Adds a new node to the AST
         * @param type The type of the node
         * @param token The token associated with the node
         * @return Index of the newly created node
         */
        uint32_t add_node(ASTNodeType type, Token token);

        /**
         * Adds a child node to a parent node
         * @param parent_idx Index of the parent node
         * @param child_idx Index of the child node
         */
        void add_child(uint32_t parent_idx, uint32_t child_idx);

        /**
         * Dumps the AST to the specified output stream
         * @param os Output stream (defaults to std::cout)
         * @param node_idx Starting node index (defaults to 0)
         * @param indent Initial indentation level (defaults to 0)
         */
        void dump(std::ostream &os = std::cout, uint32_t node_idx = 0, size_t indent = 0) const;

    private:
        /**
         * Recursive helper function for dumping nodes
         * @param os Output stream
         * @param node_idx Current node index
         * @param indent Current indentation level
         * @param visited Tracks visited nodes to prevent cycles
         */
        void dump_node(std::ostream &os, uint32_t node_idx, size_t indent, std::vector<bool> &visited) const;

        /**
         * Helper functions for dumping various components
         */
        static void dump_token_flags(std::ostream &os, TokenFlags flags);

        static void dump_node_flags(std::ostream &os, ASTNodeFlags flags);

        static const char *node_type_to_string(ASTNodeType type);
    };

    struct ColorCode
    {
        static constexpr auto RESET = "\033[0m";
        static constexpr auto RED = "\033[31m";
        static constexpr auto GREEN = "\033[32m";
        static constexpr auto YELLOW = "\033[33m";
        static constexpr auto BLUE = "\033[34m";
        static constexpr auto MAGENTA = "\033[35m";
        static constexpr auto CYAN = "\033[36m";
        static constexpr auto BOLD = "\033[1m";
    };
}
