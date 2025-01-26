// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#pragma once

#include <cstdint>
#include <vector>

namespace orx::compiler
{
    enum class TokenType : uint8_t
    {
        // keywords
        TRUE,
        FALSE,
        NIL,
        IMPORT,
        VAR,
        CONST,
        FUNCTION,
        INLINE,
        RETURN,
        ENUM,
        IF,
        ELSE,
        FOR,
        WHILE,
        BREAK,
        CONTINUE,
        SWITCH,
        CASE,
        DEFAULT,
        CLASS,
        FINAL,
        PUBLIC,
        PRIVATE,
        STATIC,
        AWAIT,
        ASYNC,
        TRY,
        CATCH,
        FROM,
        AS,
        OPERATOR,
        NEW,
        DELETE,

        // builtin types
        U8,
        I8,
        U16,
        I16,
        U32,
        I32,
        U64,
        I64,
        F32,
        F64,
        STRING,
        BOOL,
        VOID,
        OWN,
        SHARE,
        REF,
        PIN,

        // single operators
        PLUS, // +
        MINUS, // -
        STAR, // *
        SLASH, // /
        PERCENT, // %
        EQUAL, // =
        BANG, // !
        LESS, // <
        GREATER, // >
        AND, // &
        OR, // |
        XOR, // ^
        TILDE, // ^
        DOT, // .

        // mult-character operators
        ARROW,         // ->
        SCOPE,         // ::
        RANGE,         // ..
        SPREAD,        // ...
        LOGICAL_AND,   // &&
        LOGICAL_OR,    // ||
        GE,            // >=
        LE,            // <=
        EQ,            // ==
        NE,            // !=
        PLUS_EQ,       // +=
        MINUS_EQ,      // -=
        STAR_EQ,       // *=
        SLASH_EQ,      // /=
        PERCENT_EQ,    // %=
        AND_EQ,        // &=
        OR_EQ,         // |=
        XOR_EQ,        // ^=
        LEFT_SHIFT, // <<
        RIGHT_SHIFT, // >>
        LEFT_SHIFT_EQ, // <<=
        RIGHT_SHIFT_EQ, // >>=

        // delimiters
        LEFT_PAREN,    // (
        RIGHT_PAREN,   // )
        LEFT_BRACE,    // {
        RIGHT_BRACE,   // }
        LEFT_BRACKET,  // [
        RIGHT_BRACKET, // ]
        COMMA,         // ,
        COLON,         // :
        SEMICOLON,     // ;
        QUESTION,      // ?

        // annotations
        ALIGN_ANNOT,      // @align
        DEPRECATED_ANNOT, // @deprecated
        PACKED_ANNOT,     // @packed
        NO_DISCARD_ANNOT, // @nodiscard
        VOLATILE_ANNOT,   // @volatile
        LAZY_ANNOT,       // @lazy
        PURE_ANNOT,       // @pure
        TAIL_REC_ANNOT,   // @tailrec

        IDENTIFIER,
        NUM_LITERAL,
        STR_LITERAL,
        ANNOTATION,
        UNKNOWN,
        END_OF_FILE
    };

    enum class TokenFlags : uint8_t
    {
        NONE = 0,

        // literal errors
        UNTERMINATED_STRING = 1 << 0,
        INVALID_ESCAPE_SEQUENCE = 1 << 1,
        INVALID_DIGIT = 1 << 2,
        MULTIPLE_DECIMAL_POINTS = 1 << 3,
        INVALID_EXPONENT = 1 << 4,

        // comment errors
        UNTERMINATED_BLOCK_COMMENT = 1 << 5,

        // identifier errors
        INVALID_IDENTIFIER_START = 1 << 6,
        INVALID_IDENTIFIER_CHAR = 1 << 7
    };

    struct alignas(8) Token
    {
        uint32_t start;
        uint16_t len;
        TokenType type;
        TokenFlags flags;
    };

    struct alignas(8) TokenList
    {
        std::vector<uint32_t> starts;
        std::vector<uint16_t> lens;
        std::vector<TokenType> types;
        std::vector<TokenFlags> flags;

        void emplace_back(const Token &tk)
        {
            starts.emplace_back(tk.start);
            lens.emplace_back(tk.len);
            types.emplace_back(tk.type);
            flags.emplace_back(tk.flags);
        }

        void push_back(const Token &tk)
        {
            starts.push_back(tk.start);
            lens.push_back(tk.len);
            types.push_back(tk.type);
            flags.push_back(tk.flags);
        }

        void reserve(const uint32_t &n)
        {
            starts.reserve(n);
            lens.reserve(n);
            types.reserve(n);
            flags.reserve(n);
        }

        [[nodiscard]] size_t size() const
        {
            return starts.size();
        }
    };

    static constexpr std::pair<std::string_view, TokenType> token_map[] = {
        { "true", TokenType::TRUE },
        { "false", TokenType::FALSE },
        { "null", TokenType::NIL },
        { "import", TokenType::IMPORT },
        { "var", TokenType::VAR },
        { "const", TokenType::CONST },
        { "function", TokenType::FUNCTION },
        { "inline", TokenType::INLINE },
        { "return", TokenType::RETURN },
        { "enum", TokenType::ENUM },
        { "if", TokenType::IF },
        { "else", TokenType::ELSE },
        { "for", TokenType::FOR },
        { "while", TokenType::WHILE },
        { "break", TokenType::BREAK },
        { "continue", TokenType::CONTINUE },
        { "switch", TokenType::SWITCH },
        { "case", TokenType::CASE },
        { "default", TokenType::DEFAULT },
        { "class", TokenType::CLASS },
        { "final", TokenType::FINAL },
        { "public", TokenType::PUBLIC },
        { "private", TokenType::PRIVATE },
        { "await", TokenType::AWAIT },
        { "async", TokenType::ASYNC },
        { "try", TokenType::TRY },
        { "catch", TokenType::CATCH },
        { "static", TokenType::STATIC },
        { "from", TokenType::FROM },
        { "as", TokenType::AS },
        { "operator", TokenType::OPERATOR },
        { "new", TokenType::NEW },
        { "delete", TokenType::NEW },

        { "u8", TokenType::U8 },
        { "i8", TokenType::I8 },
        { "u16", TokenType::U16 },
        { "i16", TokenType::I16 },
        { "u32", TokenType::U32 },
        { "i32", TokenType::I32 },
        { "u64", TokenType::U64 },
        { "i64", TokenType::I64 },
        { "f32", TokenType::F32 },
        { "f64", TokenType::F64 },
        { "string", TokenType::STRING },
        { "bool", TokenType::BOOL },
        { "void", TokenType::VOID },
        { "Own", TokenType::OWN },
        { "Share", TokenType::SHARE },
        { "Ref", TokenType::REF },
        { "Pin", TokenType::PIN },

        { "+", TokenType::PLUS },
        { "-", TokenType::MINUS },
        { "*", TokenType::STAR },
        { "/", TokenType::SLASH },
        { "%", TokenType::PERCENT },
        { "=", TokenType::EQUAL },
        { "!", TokenType::BANG },
        { "<", TokenType::LESS },
        { ">", TokenType::GREATER },
        { "&", TokenType::AND },
        { "|", TokenType::OR },
        { "^", TokenType::XOR },
        { "~", TokenType::TILDE },
        { ".", TokenType::DOT },

        { "(", TokenType::LEFT_PAREN },
        { ")", TokenType::RIGHT_PAREN },
        { "{", TokenType::LEFT_BRACE },
        { "}", TokenType::RIGHT_BRACE },
        { "[", TokenType::LEFT_BRACKET },
        { "]", TokenType::RIGHT_BRACKET },
        { ",", TokenType::COMMA },
        { ":", TokenType::COLON },
        { ";", TokenType::SEMICOLON },
        { "?", TokenType::QUESTION },

        { "@align", TokenType::ALIGN_ANNOT },
        { "@deprecated", TokenType::DEPRECATED_ANNOT },
        { "@packed", TokenType::PACKED_ANNOT },
        { "@nodiscard", TokenType::NO_DISCARD_ANNOT },
        { "@volatile", TokenType::VOLATILE_ANNOT },
        { "@lazy", TokenType::LAZY_ANNOT },
        { "@pure", TokenType::PURE_ANNOT },
        { "@tailrec", TokenType::TAIL_REC_ANNOT }
    };

    static constexpr auto create_reverse_map()
    {
        std::array<std::string_view, static_cast<size_t>(TokenType::END_OF_FILE) + 1> map{};

        for (const auto& [str, token] : token_map)
            map[static_cast<size_t>(token)] = str;

        map[static_cast<size_t>(TokenType::IDENTIFIER)] = "IDENTIFIER";
        map[static_cast<size_t>(TokenType::NUM_LITERAL)] = "NUM_LITERAL";
        map[static_cast<size_t>(TokenType::STR_LITERAL)] = "STR_LITERAL";
        map[static_cast<size_t>(TokenType::ANNOTATION)] = "ANNOTATION";
        map[static_cast<size_t>(TokenType::UNKNOWN)] = "UNKNOWN";
        map[static_cast<size_t>(TokenType::END_OF_FILE)] = "EOF";

        return map;
    }

    inline std::string_view token_to_str(const TokenType& t)
    {
        static constexpr auto rev_map = create_reverse_map();
        const auto i = static_cast<size_t>(t);
        return i >= rev_map.size() ? "INVALID" : rev_map[i];
    }
}
