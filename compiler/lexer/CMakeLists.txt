set(ORX_LEXER_SRC
        include/lexer.h
        src/lexer.cpp
)

add_library(ORX_LEXER STATIC ${ORX_LEXER_SRC})
