# This file is part of the Klare programming language and is licensed under MIT License;
# See LICENSE.txt for details

set(KLR_LEXER_SRC
        include/lexer.h
        src/lexer.cpp
)

add_library(klr-lexer STATIC ${KLR_LEXER_SRC})

target_include_directories(klr-lexer
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)