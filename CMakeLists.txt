cmake_minimum_required(VERSION 3.15)
project(cttrie)

set(CMAKE_CXX_STANDARD 17)
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif ()

set(TEST_SOURCES
    test/catch_main.cpp
    test/test_cttrie.cpp
)
add_executable(cttrie_test ${TEST_SOURCES})

target_include_directories(cttrie_test PRIVATE ${PROJECT_SOURCE_DIR})
