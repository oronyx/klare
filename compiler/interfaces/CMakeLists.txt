add_library(ORX_INTERFACE INTERFACE
        include/tokens.h
)

target_include_directories(ORX_INTERFACE
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)