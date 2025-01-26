add_executable(oronyx-test
        ${CMAKE_SOURCE_DIR}/vendor/catch2.cpp
        tokenize/basic.cpp
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