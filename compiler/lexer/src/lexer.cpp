// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#include "../include/lexer.h"

namespace orx::compiler
{
    // zero branch lookup tb for char classification
    // 1 = ws
    // 2 = comment start
    // 3 = comment continuation
    // 4 == identifier
    // 5 == digit
    // 6 == str delimiter
    static const std::array<uint8_t, 256> char_type = []
    {
        std::array<uint8_t, 256> types {};
        for (auto i = 0; i < 256; ++i)
        {
            types[i] = (i == ' ' || i == '\t' || i == '\n' || i == '\r') * 1 +
                       (i == '/') * 2 +
                       (i == '*') * 3 +
                       (std::isalpha(i) || i == '_' || i == '@') * 4 +
                       std::isdigit(i) * 5 +
                       (i == '"') * 6;
        }
        return types;
    }();

    static constexpr std::array<TokenType, 256> single_char_tokens = []
    {
        // align data + 256 bytes for SIMD
        alignas(64) std::array<TokenType, 256> tokens {};
        std::ranges::fill(tokens, TokenType::UNKNOWN);

        tokens['+'] = TokenType::PLUS;
        tokens['-'] = TokenType::MINUS;
        tokens['*'] = TokenType::STAR;
        tokens['/'] = TokenType::SLASH;
        tokens['%'] = TokenType::PERCENT;
        tokens['='] = TokenType::EQUAL;
        tokens['!'] = TokenType::BANG;
        tokens['<'] = TokenType::LESS;
        tokens['>'] = TokenType::GREATER;
        tokens['&'] = TokenType::AND;
        tokens['|'] = TokenType::OR;
        tokens['^'] = TokenType::XOR;
        tokens['~'] = TokenType::TILDE;
        tokens['.'] = TokenType::DOT;
        tokens['('] = TokenType::LEFT_PAREN;
        tokens[')'] = TokenType::RIGHT_PAREN;
        tokens['{'] = TokenType::LEFT_BRACE;
        tokens['}'] = TokenType::RIGHT_BRACE;
        tokens['['] = TokenType::LEFT_BRACKET;
        tokens[']'] = TokenType::RIGHT_BRACKET;
        tokens[','] = TokenType::COMMA;
        tokens[':'] = TokenType::COLON;
        tokens[';'] = TokenType::SEMICOLON;
        tokens['?'] = TokenType::QUESTION;

        return tokens;
    }();

    static constexpr std::array<uint8_t, 256> hex_lookup = []
    {
        std::array<uint8_t, 256> table {};
        for (int i = '0'; i <= '9'; ++i)
            table[i] = 1;
        for (int i = 'a'; i <= 'f'; ++i)
            table[i] = 1;
        for (int i = 'A'; i <= 'F'; ++i)
            table[i] = 1;
        return table;
    }();

    static constexpr std::array<uint8_t, 256> bin_lookup = []
    {
        std::array<uint8_t, 256> table {};
        table['0'] = table['1'] = 1;
        return table;
    }();

    static constexpr std::array<uint8_t, 256> valid_escapes = []
    {
        std::array<uint8_t, 256> table {};
        table['n'] = table['t'] = table['r'] = table['\\'] =
                                               table['"'] = table['0'] = table['x'] = 1;
        return table;
    }();

    constexpr uint8_t make_flag(const bool condition, TokenFlags flag)
    {
        return condition ? static_cast<uint8_t>(flag) : 0;
    }

    Lexer::Lexer(const std::string_view src) : src(src.data(), src.size())
                                               , current_pos(0)
                                               , src_length(src.length())
    {
        // TODO: May or may not to figure out how to effectively
        // estimate the size if the needed tokens
        tokens.reserve(src.length() / 4);
        line_starts.reserve(src.length() / 40);
        line_starts.emplace_back(0);
    }

    void Lexer::skip_whitespace_comment()
    {
        // fast path for more than 8 chars
        // that compiles to a guaranteed SIMD
        while (current_pos + 8 <= src_length)
        {
            uint64_t chunk;
            memcpy(&chunk, src.data() + current_pos, sizeof(chunk));
            const uint64_t ws_mask = (chunk == 0x2020202020202020ULL) |                           // spaces
                                     (chunk == 0x0909090909090909ULL) |                           // tabs
                                     ((chunk & 0xDFDFDFDFDFDFDFDFULL) == 0x0A0A0A0A0A0A0A0AULL) | // \n
                                     ((chunk & 0xDFDFDFDFDFDFDFDFULL) == 0x0D0D0D0D0D0D0D0DULL);  // \r

            if (ws_mask == 0xFFFFFFFFFFFFFFFFULL)
            {
                if (const uint64_t newline_mask = (chunk & 0xDFDFDFDFDFDFDFDFULL) == 0x0A0A0A0A0A0A0A0AULL)
                {
                    const uint64_t newline_positions = newline_mask & chunk;
                    for (auto i = 0; i < 8; ++i)
                    {
                        if (newline_positions & (1ULL << (i * 8)))
                        {
                            line_starts.emplace_back(current_pos + i + 1);
                        }
                    }
                }

                current_pos += 8;
                continue;
            }

            break;
        }

    // slow path, usually runs when it is near the EOF
    while
    (current_pos<src_length)
    {
            const char current_char = src[current_pos];
            const uint8_t type = char_type[static_cast<uint8_t>(current_char)];
            if (current_char == '\n')
                line_starts.emplace_back(current_pos + 1);

            const bool has_next = current_pos + 1 < src_length;
            const char next_char = has_next ? src[current_pos + 1] : '\0';
            const uint32_t is_slash = type == 2;
            const uint32_t is_single_comment = is_slash & (next_char == '/');
            const uint32_t is_multi_comment = is_slash & (next_char == '*');
            current_pos += 2 * is_single_comment;
            while (is_single_comment && current_pos < src_length && src[current_pos] != '\n')
                ++current_pos;

            current_pos += 2 * is_multi_comment;
            uint32_t in_comment = is_multi_comment;
            while (in_comment && current_pos + 1 < src_length)
            {
                const uint32_t eoc = src[current_pos] == '*' & src[current_pos + 1] == '/';
                if (src[current_pos] == '\n')
                    line_starts.emplace_back(current_pos + 1);

                current_pos += 1 + eoc;
                in_comment &= !eoc;
            }

            const uint32_t should_continue = type == 1;
            current_pos += should_continue;
            if (!(should_continue | is_single_comment | is_multi_comment))
                return;
        }
    }

    Token Lexer::lex_identifier() const
    {
        const char *start = src.data() + current_pos;
        const char *current = start;
        uint8_t flags = 0;

        const uint32_t is_valid_start = (*current == '_' || *current == '@' ||
                                         std::isalpha(*current));
        flags |= make_flag(!is_valid_start, TokenFlags::INVALID_IDENTIFIER_START);

        current += (*current == '@');

        while (current < src.data() + src_length)
        {
            const char c = *current;
            const uint32_t is_alnum = std::isalnum(c);
            const uint32_t is_underscore = c == '_';
            const uint32_t is_space = std::isspace(c);
            const uint32_t is_punct = std::ispunct(c);

            const uint32_t is_valid = is_alnum | is_underscore;
            const uint32_t is_terminator = (!is_valid) & (is_space | is_punct);

            flags |= (!is_valid & !is_terminator) *
                    static_cast<uint8_t>(TokenFlags::INVALID_IDENTIFIER_CHAR);

            current += is_valid;
            if (!is_valid)
                break;
        }

        const uint16_t length = current - start;
        const std::string_view text(start, length);

        for (const auto &[next, type]: token_map)
        {
            if (next.length() == length &&
                memcmp(next.data(), text.data(), length) == 0)
            {
                return { current_pos, length, type, static_cast<TokenFlags>(flags) };
            }
        }

        return { current_pos, length, TokenType::IDENTIFIER, static_cast<TokenFlags>(flags) };
    }

    Token Lexer::lex_number() const
    {
        const char *start = src.data() + current_pos;
        const char *current = start;
        const char *end = src.data() + src_length;
        uint8_t flags = 0;

        if (*current >= '0' && *current <= '9')
        {
            while (current + 8 <= end)
            {
                uint64_t chunk;
                memcpy(&chunk, current, sizeof(chunk));
                const uint64_t digit_test = chunk - 0x3030303030303030ULL;
                const uint64_t digit_test2 = chunk + 0x4646464646464646ULL;
                current += 8 * !((digit_test | digit_test2) & 0x8080808080808080ULL);
                if ((digit_test | digit_test2) & 0x8080808080808080ULL)
                    break;
            }
        }

        const uint32_t has_next = current + 1 < end;
        const char next = current[has_next];
        const uint32_t is_zero = *current == '0';
        const uint32_t is_x = (next | 32) == 'x';
        const uint32_t is_b = (next | 32) == 'b';
        const uint32_t is_hex = is_zero & is_x;
        const uint32_t is_bin = is_zero & is_b;

        current += 2 * (is_hex | is_bin);
        uint32_t has_decimal = 0;
        while (current < end)
        {
            const char c = *current;
            const uint32_t is_digit = (c >= '0' && c <= '9');
            const uint32_t is_dot = c == '.';
            const uint32_t is_valid_hex = hex_lookup[static_cast<uint8_t>(c)];
            const uint32_t is_valid_bin = bin_lookup[static_cast<uint8_t>(c)];
            const uint32_t is_valid = (is_hex & is_valid_hex) |
                                      (is_bin & is_valid_bin) |
                                      ((!is_hex & !is_bin) & (is_digit | is_dot));

            if (!is_valid)
                break;

            has_decimal += is_dot;
            flags |= (has_decimal > 1) * static_cast<uint8_t>(TokenFlags::MULTIPLE_DECIMAL_POINTS);

            ++current;
        }

        const uint32_t at_exp = current < end;
        const uint32_t is_exp = at_exp & ((*current | 32) == 'e');
        current += is_exp;

        const uint32_t has_sign = (current < end) & ((*current == '+') | (*current == '-'));
        current += is_exp * has_sign;

        const uint32_t exp_valid = (current < end) & ((*current >= '0') & (*current <= '9'));
        flags |= (is_exp & !exp_valid) * static_cast<uint8_t>(TokenFlags::INVALID_EXPONENT);

        const uint32_t should_continue = is_exp & exp_valid;
        while (should_continue & (current < end) & ((*current >= '0') & (*current <= '9')))
            ++current;

        return {
            current_pos,
            static_cast<uint16_t>(current - start),
            TokenType::NUM_LITERAL,
            static_cast<TokenFlags>(flags)
        };
    }

    Token Lexer::lex_operator() const
    {
        const char *start = src.data() + current_pos;
        const char *current = start;
        const char *end = src.data() + src_length;
        uint8_t flags = 0;

        const char next = (current + 1 < end) ? *(current + 1) : '\0';
        switch (*current)
        {
            case '-':
                if (next == '>')
                    return { current_pos, 2, TokenType::ARROW, static_cast<TokenFlags>(flags) };
                if (next == '=')
                    return { current_pos, 2, TokenType::MINUS_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '=':
                if (next == '=')
                    return { current_pos, 2, TokenType::EQ, static_cast<TokenFlags>(flags) };
                break;
            case ':':
                if (next == ':')
                    return { current_pos, 2, TokenType::SCOPE, static_cast<TokenFlags>(flags) };
                break;
            case '.':
                if (next == '.')
                {
                    const char third = (current + 2 < end) ? *(current + 2) : '\0';
                    if (third == '.')
                        return { current_pos, 3, TokenType::SPREAD, static_cast<TokenFlags>(flags) };
                    return { current_pos, 2, TokenType::RANGE, static_cast<TokenFlags>(flags) };
                }
                break;
            case '&':
                if (next == '&')
                    return { current_pos, 2, TokenType::LOGICAL_AND, static_cast<TokenFlags>(flags) };
                if (next == '=')
                    return { current_pos, 2, TokenType::AND_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '|':
                if (next == '|')
                    return { current_pos, 2, TokenType::LOGICAL_OR, static_cast<TokenFlags>(flags) };
                if (next == '=')
                    return { current_pos, 2, TokenType::OR_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '>':
                if (next == '=')
                    return { current_pos, 2, TokenType::GE, static_cast<TokenFlags>(flags) };
                if (next == '>')
                {
                    const char third = (current + 2 < end) ? *(current + 2) : '\0';
                    if (third == '=')
                        return { current_pos, 3, TokenType::RIGHT_SHIFT_EQ, static_cast<TokenFlags>(flags) };
                    else
                        return { current_pos, 2, TokenType::RIGHT_SHIFT, static_cast<TokenFlags>(flags) };
                }
                break;
            case '<':
                if (next == '=')
                    return { current_pos, 2, TokenType::LE, static_cast<TokenFlags>(flags) };
                if (next == '<')
                {
                    const char third = (current + 2 < end) ? *(current + 2) : '\0';
                    if (third == '=')
                        return { current_pos, 3, TokenType::LEFT_SHIFT_EQ, static_cast<TokenFlags>(flags) };
                    else
                        return { current_pos, 2, TokenType::LEFT_SHIFT, static_cast<TokenFlags>(flags) };
                }
                break;
            case '!':
                if (next == '=')
                    return { current_pos, 2, TokenType::NE, static_cast<TokenFlags>(flags) };
                break;
            case '+':
                if (next == '=')
                    return { current_pos, 2, TokenType::PLUS_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '*':
                if (next == '=')
                    return { current_pos, 2, TokenType::STAR_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '/':
                if (next == '=')
                    return { current_pos, 2, TokenType::SLASH_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '%':
                if (next == '=')
                    return { current_pos, 2, TokenType::PERCENT_EQ, static_cast<TokenFlags>(flags) };
                break;
            case '^':
                if (next == '=')
                    return { current_pos, 2, TokenType::XOR_EQ, static_cast<TokenFlags>(flags) };
                break;
            default:
                return {
                    current_pos, 1, single_char_tokens[static_cast<uint8_t>(*current)], static_cast<TokenFlags>(flags)
                };;
        }

        return { current_pos, 1, single_char_tokens[static_cast<uint8_t>(*current)], static_cast<TokenFlags>(flags) };
    }

    Token Lexer::lex_string() const
    {
        const char *start = src.data() + current_pos;
        const char *current = start + 1;
        const char *end = src.data() + src_length;
        uint8_t flags = 0;

        while (current < end)
        {
            const char c = *current;
            const uint32_t is_escape = c == '\\';
            const uint32_t has_next = current + 1 < end;
            const char next = current[has_next];
            const uint32_t is_quote = c == '"';
            const uint32_t is_valid_escape = valid_escapes[static_cast<uint8_t>(next)];
            const uint32_t is_x_escape = is_escape & (next == 'x');
            if (is_x_escape)
            {
                // if enough escape chars (3) i.e. \x123
                if (current + 3 >= end || !hex_lookup[static_cast<uint8_t>(current[1])] || !hex_lookup[static_cast<uint8_t>(current[2])])
                {
                    flags |= static_cast<uint8_t>(TokenFlags::INVALID_ESCAPE_SEQUENCE);
                    break;
                }
            }

            flags |= make_flag(is_escape & !is_valid_escape, TokenFlags::INVALID_ESCAPE_SEQUENCE);
            current += 1 + (is_escape * (1 + (is_x_escape * 2)));
            if (is_quote)
                break;
        }

        flags |= make_flag(current >= end || *(current - 1) != '"', TokenFlags::UNTERMINATED_STRING);
        return {
            current_pos,
            static_cast<uint16_t>(current - start),
            TokenType::STR_LITERAL,
            static_cast<TokenFlags>(flags)
        };
    }

    Token Lexer::next_token()
    {
        skip_whitespace_comment();

        if (current_pos >= src_length)
            return { current_pos, 0, TokenType::END_OF_FILE, static_cast<TokenFlags>(0) };

        switch (const char c = src[current_pos]; char_type[static_cast<uint8_t>(c)])
        {
            case 4:
                return lex_identifier();
            case 5:
                return lex_number();
            case 6:
                return lex_string();
            default:
                return { current_pos, 1, single_char_tokens[static_cast<uint8_t>(c)], static_cast<TokenFlags>(0) };
        }
    }

    // returns a unique_ptr of TokenList because
    // the parser needs it to process obv
    std::unique_ptr<TokenList> Lexer::tokenize()
    {
        uint32_t state = 1;
        while (state)
        {
            Token t = next_token();
            tokens.emplace_back(t);

            state = t.type != TokenType::END_OF_FILE;
            current_pos += t.len * state;
        }
        return std::make_unique<TokenList>(tokens);
    }

    // returns a unique_ptr of uint32_t OR line because
    // the parser needs to produce efficient errors
    std::unique_ptr<std::vector<uint32_t>> Lexer::get_line_starts()
    {
        return std::make_unique<std::vector<uint32_t>>(line_starts);
    }
}