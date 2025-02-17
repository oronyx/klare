# This file is part of the Klare programming language and is licensed under MIT License;
# See LICENSE.txt for details

add_executable(klr-test
        # vendor
        ${CMAKE_SOURCE_DIR}/vendor/catch2.cpp

        # tokenize
        tokenize/unit/annotations.cpp
        tokenize/unit/classes.cpp
        tokenize/unit/comments.cpp
        tokenize/unit/control_flows.cpp
        tokenize/unit/delimiters.cpp
        tokenize/unit/eof.cpp
        tokenize/unit/identifiers.cpp
        tokenize/unit/keywords.cpp
        tokenize/unit/multi_ops.cpp
        tokenize/unit/num_literals.cpp
        tokenize/unit/single_ops.cpp
        tokenize/unit/str_literals.cpp
        tokenize/integration/var_decl.cpp
        tokenize/integration/function_decl.cpp

        tokenize/integration/control_flows.cpp
        tokenize/integration/classes.cpp
        tokenize/integration/complex.cpp

        # parsing
        parsing/unit/arr_decl.cpp
        parsing/unit/arr_index.cpp
        parsing/unit/bin_expr.cpp
        parsing/unit/call_expr.cpp
        parsing/unit/const_decl.cpp
        parsing/unit/ptr_decl.cpp
        parsing/unit/una_expr.cpp
        parsing/unit/var_decl.cpp
)

target_include_directories(klr-test PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(klr-test PRIVATE
        klr
)

set_target_properties(klr-test
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)