cmake_minimum_required(VERSION 3.12.4)

set(PROJECT_BIN "project_1")
project(${PROJECT_BIN} C)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# Executable output location (keep your original style)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Warnings (optional)
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add project root to the header search path
include_directories(${PROJECT_SOURCE_DIR}/)

# cJSON library (static)
# Assuming your layout:
#   inc/cJSON.h, inc/cJSON_Utils.h
#   src/cJSON.c, src/cJSON_Utils.c
add_library(cjson STATIC
  ${PROJECT_SOURCE_DIR}/src/cJSON.c
  ${PROJECT_SOURCE_DIR}/src/cJSON_Utils.c
)
# Find cJSON headers
target_include_directories(cjson PUBLIC
  ${PROJECT_SOURCE_DIR}/inc
)

# Main executable
add_executable(${PROJECT_BIN}
  ${PROJECT_SOURCE_DIR}/main.c
  # Add additional source files here if needed.
  
)

# Make the target aware of 'inc/' so #include "cJSON.h" resolves
target_include_directories(${PROJECT_BIN} PRIVATE
  ${PROJECT_SOURCE_DIR}/inc
)

# Link with cJSON library
target_link_libraries(${PROJECT_BIN} PRIVATE cjson)

# Copy data.json next to the exe after build
# data/data.json -> bin/data.json
add_custom_command(TARGET ${PROJECT_BIN} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${PROJECT_SOURCE_DIR}/data/data.json"
          "${EXECUTABLE_OUTPUT_PATH}/data.json"
)

# Custom target to run the executable
add_custom_target(run
  COMMAND "${EXECUTABLE_OUTPUT_PATH}/${PROJECT_BIN}"
  DEPENDS ${PROJECT_BIN}
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)
