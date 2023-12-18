set(CUR_TARGET "shading")
FILE(GLOB CUR_SRC "*.cpp")
add_executable(${PROJECT_NAME}_${CUR_TARGET}
	${CUR_SRC}
)

install(DIRECTORY ./shadingshaders
		DESTINATION bin )