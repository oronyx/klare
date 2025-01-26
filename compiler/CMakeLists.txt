add_subdirectory(lexer)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
add_library(ORX_COMPILER STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

target_link_libraries(ORX_COMPILER
        PUBLIC
        ORX_INTERFACE
        ORX_LEXER
)