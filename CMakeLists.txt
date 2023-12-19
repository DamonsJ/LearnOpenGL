cmake_minimum_required(VERSION 3.20)

include(FetchContent)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.9
)

FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)

    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

FetchContent_Declare(
        linmath
        GIT_REPOSITORY https://github.com/datenwolf/linmath.h.git
)

FetchContent_GetProperties(linmath)
if(NOT linmath_POPULATED)
    FetchContent_Populate(linmath)
    add_library(linmath INTERFACE)
    target_include_directories(linmath INTERFACE ${linmath_SOURCE_DIR})
endif()

FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v2.0.4
)

FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
    add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR})
    glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)
endif()


FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()


FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
)

FetchContent_GetProperties(assimp)
if(NOT assimp_POPULATED)
    FetchContent_Populate(assimp)
    add_subdirectory(${assimp_SOURCE_DIR} ${assimp_BINARY_DIR})
endif()


link_libraries(glfw linmath glad_gl_core_33 glm assimp)
add_subdirectory(common)
link_libraries(common)
add_subdirectory(src)

function(install_all_targets DIR)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(TGT IN LISTS TGTS)
        message(STATUS "Target: ${TGT}")
        install(TARGETS ${TGT}
                RUNTIME DESTINATION bin
                LIBRARY DESTINATION lib
                ARCHIVE DESTINATION lib)
    endforeach()

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
    install_all_targets("${SUBDIR}")
    endforeach()
endfunction()

install_all_targets(./src)