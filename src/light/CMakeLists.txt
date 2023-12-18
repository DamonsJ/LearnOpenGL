set(CUR_TARGET "light")
FILE(GLOB CUR_SRC "*.cpp")
add_executable(${PROJECT_NAME}_${CUR_TARGET}
	${CUR_SRC}
)

install(DIRECTORY ./lightshaders
		DESTINATION bin )