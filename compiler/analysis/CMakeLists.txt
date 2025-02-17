# This file is part of the Klare programming language and is licensed under MIT License;
# See LICENSE.txt for details

set(KLR_ANALYSIS_SRC
        include/ast.h
        src/ast.cpp
)

add_library(klr-analysis STATIC ${KLR_ANALYSIS_SRC})

target_include_directories(klr-analysis
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)