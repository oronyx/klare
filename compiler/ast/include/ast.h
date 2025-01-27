// This file is part of the Oronyx programming language and is licensed under MIT License;
// See LICENSE.txt for details

#pragma once

#include <cstdint>
#include <vector>
#include <string_view>
#include <variant>

namespace orx::compiler
{
    enum class ExprType: uint8_t
    {
        LITERAL,
        BINARY,
        UNARY,
        GROUPING,
        VARIABLE,
        FUNCTION,
        CALL,
        ARRAY,
        MEMBER,
        INDEX,
        LAMBDA,
        CONDITIONAL
    };

    struct LambdaExpr
    {
        std::vector<std::tuple<std::string_view, uint32_t, bool>> params;
        uint32_t return_type{};

        LambdaExpr() = default;
        LambdaExpr(uint32_t return_type, std::vector<std::tuple<std::string_view, uint32_t, bool>> params)
            : params(std::move(params)), return_type(return_type) {}
        LambdaExpr(const LambdaExpr&) = default;
        LambdaExpr(LambdaExpr&&) = default;
        LambdaExpr& operator=(const LambdaExpr&) = default;
        LambdaExpr& operator=(LambdaExpr&&) = default;
        ~LambdaExpr() = default;
    };

    struct ArrayAccessExpr
    {
        uint64_t array;
        uint64_t index;

        ArrayAccessExpr() = default;
        ArrayAccessExpr(const uint64_t array, const uint64_t index) : array(array), index(index) {}
        ArrayAccessExpr(const ArrayAccessExpr&) = default;
        ArrayAccessExpr(ArrayAccessExpr&&) = default;
        ArrayAccessExpr& operator=(const ArrayAccessExpr&) = default;
        ArrayAccessExpr& operator=(ArrayAccessExpr&&) = default;
        ~ArrayAccessExpr() = default;
    };

    struct LiteralExpr
    {
        uint32_t value_idx;
        uint8_t type_tag;

        LiteralExpr() = default;
        LiteralExpr(const LiteralExpr&) = default;
        LiteralExpr(LiteralExpr&&) = default;
        LiteralExpr& operator=(const LiteralExpr&) = default;
        LiteralExpr& operator=(LiteralExpr&&) = default;
        ~LiteralExpr() = default;
    };

    struct BinaryExpr
    {
        uint32_t left;
        uint32_t right;
        uint8_t op;

        BinaryExpr() = default;
        BinaryExpr(const uint32_t left, const uint32_t right, const uint8_t op) : left(left), right(right), op(op) {}
        BinaryExpr(const BinaryExpr&) = default;
        BinaryExpr(BinaryExpr&&) = default;
        BinaryExpr& operator=(const BinaryExpr&) = default;
        BinaryExpr& operator=(BinaryExpr&&) = default;
        ~BinaryExpr() = default;
    };

    struct UnaryExpr
    {
        uint32_t expr;
        uint8_t op;

        UnaryExpr() = default;
        UnaryExpr(const uint32_t expr, const uint8_t op) : expr(expr), op(op) {}
        UnaryExpr(const UnaryExpr&) = default;
        UnaryExpr(UnaryExpr&&) = default;
        UnaryExpr& operator=(const UnaryExpr&) = default;
        UnaryExpr& operator=(UnaryExpr&&) = default;
        ~UnaryExpr() = default;
    };

    struct ConditionalExpr
    {
        size_t condition;
        size_t true_branch;
        size_t false_branch;

        ConditionalExpr() = default;
        ConditionalExpr(size_t condition, size_t true_branch, size_t false_branch)
            : condition(condition), true_branch(true_branch), false_branch(false_branch) {}
        ConditionalExpr(const ConditionalExpr&) = default;
        ConditionalExpr(ConditionalExpr&&) = default;
        ConditionalExpr& operator=(const ConditionalExpr&) = default;
        ConditionalExpr& operator=(ConditionalExpr&&) = default;
        ~ConditionalExpr() = default;
    };

    struct CallExpr
    {
        uint32_t callee{};
        std::vector<uint32_t> args;

        CallExpr() = default;
        CallExpr(const CallExpr&) = default;
        CallExpr(CallExpr&&) = default;
        CallExpr& operator=(const CallExpr&) = default;
        CallExpr& operator=(CallExpr&&) = default;
        ~CallExpr() = default;

        CallExpr(int i, const std::vector<unsigned> & move) : callee(i), args(move.begin(), move.end()) {};
    };

    struct ArrayExpr
    {
        uint32_t type_idx{};
        std::vector<uint32_t> elements;

        ArrayExpr() = default;
        ArrayExpr(const ArrayExpr&) = default;
        ArrayExpr(ArrayExpr&&) = default;
        ArrayExpr& operator=(const ArrayExpr&) = default;
        ArrayExpr& operator=(ArrayExpr&&) = default;
        ~ArrayExpr() = default;
    };

    struct MemberExpr
    {
        uint32_t object;
        uint32_t member;

        MemberExpr() = default;
        MemberExpr(const MemberExpr&) = default;
        MemberExpr(MemberExpr&&) = default;
        MemberExpr& operator=(const MemberExpr&) = default;
        MemberExpr& operator=(MemberExpr&&) = default;
        ~MemberExpr() = default;
    };

    struct Expr
    {
        ExprType type{};
        uint32_t pos{};

        std::variant<LiteralExpr, BinaryExpr, UnaryExpr, CallExpr, ArrayExpr, MemberExpr, ArrayAccessExpr, LambdaExpr, ConditionalExpr> data;

        template<typename T>
        Expr(const ExprType t, const uint32_t p, T&& expr) : type(t), pos(p), data(std::forward<T>(expr)) {}

        Expr() = default;
        Expr(const Expr&) = default;
        Expr(Expr&&) = default;
        Expr& operator=(const Expr&) = default;
        Expr& operator=(Expr&&) = default;
        ~Expr() = default;
    };

    struct AST
    {
        std::vector<Expr> nodes;
        std::vector<std::string_view> strings;
        std::vector<uint32_t> types;
        std::vector<uint32_t> generic_types;

        AST() = default;
        AST(const AST&) = default;
        AST(AST&&) = default;
        AST& operator=(const AST&) = default;
        AST& operator=(AST&&) = default;
        ~AST() = default;
    };
}