add_library(orx-interface INTERFACE
        include/tokens.h
)

target_include_directories(orx-interface
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)