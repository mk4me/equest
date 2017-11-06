BEGIN_PROJECT("executable")


FILE(GLOB objFiles  "${CMAKE_BINARY_DIR}/src/plugins/imuCostume/plugin_imuCostume.dir/Debug/*.obj")

set (TARGET_OBJ ${objFiles})

SET_SOURCE_FILES(
	test.cpp
	ImuParsersTest.cpp
)

SET_PRIVATE_HEADERS(
	ImuParsersTest.h
)

END_PROJECT()