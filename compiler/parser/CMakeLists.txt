# This file is part of the Klare programming language and is licensed under MIT License;
# See LICENSE.txt for details

set(KLR_PARSER_SRC
        include/parser.h

        src/parser.cpp
)

add_library(klr-parser STATIC ${KLR_PARSER_SRC})
target_include_directories(klr-parser
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)