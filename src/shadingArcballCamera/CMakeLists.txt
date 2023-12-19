set(CUR_TARGET "arcballshading")
FILE(GLOB CUR_SRC "*.cpp")
add_executable(${PROJECT_NAME}_${CUR_TARGET}
	${CUR_SRC}
)
