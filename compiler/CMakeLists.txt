add_subdirectory(lexer)
add_subdirectory(interfaces)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")
add_library(orx STATIC ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)

target_link_libraries(orx
        PUBLIC
        ORX_INTERFACE
        ORX_LEXER
)