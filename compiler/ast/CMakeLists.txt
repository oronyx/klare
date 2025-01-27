# This file is part of the Oronyx programming language and is licensed under MIT License;
# See LICENSE.txt for details

set(ORX_AST_SRC
        include/ast.h
        src/ast.cpp
)

add_library(orx-ast STATIC ${ORX_AST_SRC})