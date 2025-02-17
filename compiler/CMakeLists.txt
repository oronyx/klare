# This file is part of the Klare programming language and is licensed under MIT License;
# See LICENSE.txt for details

add_subdirectory(analysis)
add_subdirectory(interfaces)
add_subdirectory(lexer)
add_subdirectory(parser)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
add_library(klr STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

target_link_libraries(klr
        PUBLIC
        klr-analysis
        klr-interface
        klr-lexer
        klr-parser
)