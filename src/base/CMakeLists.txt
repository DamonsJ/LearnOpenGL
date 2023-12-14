set(LIGHT_TARGET "base")
FILE(GLOB LIGHT_SRC "*.cpp")
add_executable(${PROJECT_NAME}_${LIGHT_TARGET}
	${LIGHT_SRC}
)

install(DIRECTORY ./baseshaders
		DESTINATION bin )