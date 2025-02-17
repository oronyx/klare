add_library(klr-interface INTERFACE
        include/tokens.h
)

target_include_directories(klr-interface
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
