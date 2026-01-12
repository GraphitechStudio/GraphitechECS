cmake_minimum_required(VERSION 3.15)

# Ensure GLM include path is available
# Import CPM to get glm_SOURCE_DIR
if(NOT DEFINED glm_SOURCE_DIR)
    # Find it ourselves if not set by parent
    find_package(glm QUIET)
endif()

# GMapLoader target - part of ECS library
add_library(GMapLoader OBJECT
    GMapLoader.cpp
    GMapLoader.hpp
    GMapCompiler.cpp
    GMapCompiler.hpp
)

target_include_directories(GMapLoader PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

# Add GLAD include directory if available
if(TARGET glad_lib)
    target_include_directories(GMapLoader PUBLIC "${GLAD_OUT_DIR}/include")
    target_link_libraries(GMapLoader PUBLIC glad_lib)
endif()

# ECS must not include Engine headers; UI integration should live in Engine.

# Add only the include directories for GLM (don't link it - it's header-only)
if(DEFINED glm_SOURCE_DIR)
    target_include_directories(GMapLoader PUBLIC ${glm_SOURCE_DIR})
else()
    # Fallback for when glm_SOURCE_DIR is not set
    target_include_directories(GMapLoader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.cpm-cache/glm/*/glm)
endif()

# nlohmann_json is header-only; include its headers when available
if(DEFINED nlohmann_json_SOURCE_DIR)
    target_include_directories(GMapLoader PUBLIC ${nlohmann_json_SOURCE_DIR}/include)
endif()

# Export for parent
set_target_properties(GMapLoader PROPERTIES
    FOLDER "ECS/Loaders"
)

# ============ GMAP Compiler Executable ============
add_executable(gmap-compiler
    gmap_compiler_main.cpp
    GMapCompiler.cpp
    GMapCompiler.hpp
)

if(DEFINED nlohmann_json_SOURCE_DIR)
    target_include_directories(gmap-compiler PRIVATE ${nlohmann_json_SOURCE_DIR}/include)
endif()

target_include_directories(gmap-compiler PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

# Add GLM include directories if available
if(TARGET glm)
    get_target_property(GLM_INCLUDE_DIRS glm INTERFACE_INCLUDE_DIRECTORIES)
    if(GLM_INCLUDE_DIRS)
        target_include_directories(GMapLoader PUBLIC ${GLM_INCLUDE_DIRS})
        target_include_directories(gmap-compiler PRIVATE ${GLM_INCLUDE_DIRS})
    endif()
endif()

set_target_properties(gmap-compiler PROPERTIES
    FOLDER "Tools"
)
