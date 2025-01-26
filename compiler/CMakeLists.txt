# This file is part of the Oronyx programming language and is licensed under MIT License;
# See LICENSE.txt for details

add_subdirectory(interfaces)
add_subdirectory(lexer)
add_subdirectory(parser)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
add_library(orx STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

target_link_libraries(orx
        PUBLIC
        orx-interface
        orx-lexer
        orx-parser
)