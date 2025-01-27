# This file is part of the Oronyx programming language and is licensed under MIT License;
# See LICENSE.txt for details

set(ORX_PARSER_SRC
        include/parser.h
        src/parser.cpp
        include/symbol.h
)

add_library(orx-parser STATIC ${ORX_PARSER_SRC})


