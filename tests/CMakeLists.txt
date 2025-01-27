# This file is part of the Oronyx programming language and is licensed under MIT License;
# See LICENSE.txt for details

add_executable(oronyx-test
        ${CMAKE_SOURCE_DIR}/vendor/catch2.cpp
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
        tokenize/unit/types.cpp
        tokenize/integration/var_decl.cpp
        tokenize/integration/function_decl.cpp

        parse/unit/var_decl.cpp
        tokenize/integration/control_flows.cpp
        tokenize/integration/classes.cpp
        tokenize/integration/complex.cpp
)

target_include_directories(oronyx-test PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor
)

target_link_libraries(oronyx-test PRIVATE
        orx
)

set_target_properties(oronyx-test
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)