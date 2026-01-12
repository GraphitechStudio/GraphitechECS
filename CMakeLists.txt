cmake_minimum_required(VERSION 3.15)
project(GraphitechECS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "Configuring ECS...")

# Bootstrap CPM if not available (standalone ECS build)
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

# Option to enable Engine integration (UI parsers, Engine headers)
option(GRAPHITECH_ENABLE_ENGINE "Enable Graphitech Engine integration in ECS" OFF)

# Add nlohmann/json dependency via CPM
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)
CPMAddPackage(
    NAME nlohmann_json
    VERSION 3.11.3
    GITHUB_REPOSITORY nlohmann/json
    GIT_TAG v3.11.3
    DOWNLOAD_ONLY YES
)

# GLM via CPM for standalone builds
CPMAddPackage(
    NAME glm
    GITHUB_REPOSITORY g-truc/glm
    GIT_TAG 1.0.1
    EXCLUDE_FROM_ALL ON
)

# Create a target if not already created by CPM
if(NOT TARGET glm)
    add_library(glm INTERFACE IMPORTED)
    target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
endif()

# Collect ECS source files (only .cpp files, excluding Loaders - handled separately)
file(GLOB ECS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Registry/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Components/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Factories/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tools/*.cpp
)

# Exclude UI-dependent sources when Engine integration is disabled
if(NOT GRAPHITECH_ENABLE_ENGINE)
    list(FILTER ECS_SOURCES EXCLUDE REGEX ".*/Factories/UIFactory\\.cpp$")
endif()

# Create static library for ECS
add_library(GraphitechECS STATIC ${ECS_SOURCES})

# Include directories
target_include_directories(GraphitechECS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
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

# No Engine linkage or definitions inside ECS; Engine/UI integration
# must be handled outside of ECS (e.g., in the Engine project).

message(STATUS "ECS configured")
