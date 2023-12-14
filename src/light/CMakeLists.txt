set(LIGHT_TARGET "light")
FILE(GLOB LIGHT_SRC "*.cpp")
add_executable(${PROJECT_NAME}_${LIGHT_TARGET}
	${LIGHT_SRC}
)