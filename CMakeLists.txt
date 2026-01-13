cmake_minimum_required(VERSION 3.15)
project(GraphitechECS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "Configuring ECS...")

# Add nlohmann/json dependency via CPM
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)
CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.11.3
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
    DOWNLOAD_ONLY YES
)

# Collect ECS source files (only .cpp files, excluding Loaders - handled separately)
file(GLOB ECS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Registry/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Factories/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.cpp
)

# Create static library for ECS
add_library(GraphitechECS STATIC ${ECS_SOURCES})

# Include directories
target_include_directories(GraphitechECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine
    ${nlohmann_json_SOURCE_DIR}/include
)

# If GLAD is available, add its include directory
if(TARGET glad_lib)
    target_include_directories(GraphitechECS PUBLIC "${GLAD_OUT_DIR}/include")
    target_link_libraries(GraphitechECS PUBLIC glad_lib)
endif()

# Link with dl for dynamic loading
target_link_libraries(GraphitechECS PUBLIC
    ${CMAKE_DL_LIBS}
)

# GLM is header-only; add it as an interface dependency
if(TARGET glm)
    target_link_libraries(GraphitechECS INTERFACE glm)
endif()

# Compiler flags
target_compile_options(GraphitechECS PRIVATE
    -Wall -Wextra
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Release>:-O3>
)

# Add Loaders subdirectory (contains GMapLoader and gmap-compiler tool)
add_subdirectory(Loaders)

# Link GMapLoader objects into GraphitechECS
target_link_libraries(GraphitechECS PUBLIC GMapLoader)

message(STATUS "ECS configured")
