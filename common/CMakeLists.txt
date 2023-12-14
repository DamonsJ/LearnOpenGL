set(COMMON_TARGET "common")
FILE(GLOB COMMON_SRC *.cpp *.hpp *.c *.h)

add_library(${COMMON_TARGET}
    STATIC
	${COMMON_SRC}
)

target_include_directories(${COMMON_TARGET}
                            PUBLIC
                            ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${COMMON_TARGET} PROPERTIES
                    OUTPUT_NAME common
                    VERSION 0.1
                    POSITION_INDEPENDENT_CODE ON)