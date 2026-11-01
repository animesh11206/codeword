cmake_minimum_required(VERSION 3.15)
project(codeword VERSION 1.0.0 LANGUAGES CXX)

# Require standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all .cpp source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Define the executable
add_executable(codeword ${SOURCES})

# Add header directories
target_include_directories(codeword PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/encoding"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/decoding"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/hashing"
)

# Enable compiler warnings & optimization
if(MSVC)
    target_compile_options(codeword PRIVATE /W4 /O2)
else()
    target_compile_options(codeword PRIVATE -Wall -Wextra -Wpedantic -O2)
endif()
