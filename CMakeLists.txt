cmake_minimum_required(VERSION 3.15)
project(GraphitechECS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "Configuring Graphitech ECS (STANDALONE)...")

# Configure CPM if needed
if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
    message(STATUS "Downloading CPM.cmake for ECS")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/cmake")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
        "${CMAKE_BINARY_DIR}/cmake/CPM.cmake"
        TLS_VERIFY ON
        SHOW_PROGRESS
    )
endif()

# Add nlohmann/json dependency via CPM
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

if(NOT TARGET nlohmann_json::nlohmann_json)
    CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.11.3
        GITHUB_REPOSITORY nlohmann/json
        GIT_TAG v3.11.3
        DOWNLOAD_ONLY YES
    )
endif()

# Add GLM dependency (header-only)
if(NOT TARGET glm)
    CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY g-truc/glm
        GIT_TAG 1.0.1
        EXCLUDE_FROM_ALL ON
    )
    if(NOT TARGET glm)
        add_library(glm INTERFACE IMPORTED)
        target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
    endif()
endif()

# Collect ECS source files (only .cpp files, excluding UI components which belong in Engine)
file(GLOB ECS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Registry/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Factories/EntityFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.cpp
)

# Create static library for ECS
add_library(GraphitechECS STATIC ${ECS_SOURCES})

# Include directories - NO Engine dependency!
target_include_directories(GraphitechECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${nlohmann_json_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(GraphitechECS PUBLIC
    ${CMAKE_DL_LIBS}
    glm
)

# Compiler flags
target_compile_options(GraphitechECS PRIVATE
    -Wall -Wextra
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Release>:-O3>
)

message(STATUS "✓ Graphitech ECS configured (STANDALONE - no Engine dependency)")
