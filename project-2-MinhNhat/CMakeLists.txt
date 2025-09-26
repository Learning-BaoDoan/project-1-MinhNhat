# CMakeLists.txt
cmake_minimum_required(VERSION 3.12.4)

set(PROJECT_BIN "weather_parser")
project(${PROJECT_BIN})

# Set output directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Compiler flags for better debugging and warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Create binary_io library
add_library(binary_io STATIC
    ${PROJECT_SOURCE_DIR}/src/binary_io.c
)

# Create weather_parser library  
add_library(weather_parser_lib STATIC
    ${PROJECT_SOURCE_DIR}/src/weather_parser.c
)

# Create json_writer library
add_library(json_writer STATIC
    ${PROJECT_SOURCE_DIR}/src/json_writer.c
)

# Link libraries together
target_link_libraries(weather_parser_lib binary_io json_writer)

# Create main executable
add_executable(${PROJECT_BIN}
    ${PROJECT_SOURCE_DIR}/src/main.c
)

# Link executable with libraries
target_link_libraries(${PROJECT_BIN} weather_parser_lib binary_io json_writer)

# Custom targets for convenience
add_custom_target(run 
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_BIN}
    DEPENDS ${PROJECT_BIN}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(run_sample
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_BIN} sample_data.bin output/weather_data.json
    DEPENDS ${PROJECT_BIN}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# Create directories if they don't exist
file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
file(MAKE_DIRECTORY ${LIBRARY_OUTPUT_PATH})

# Print build information
message(STATUS "Project: ${PROJECT_BIN}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Executable output: ${EXECUTABLE_OUTPUT_PATH}")
message(STATUS "Library output: ${LIBRARY_OUTPUT_PATH}")