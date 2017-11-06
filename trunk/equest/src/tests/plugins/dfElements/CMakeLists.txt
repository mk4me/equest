BEGIN_PROJECT("executable")


FILE(GLOB objFiles  "${CMAKE_BINARY_DIR}/src/plugins/dfElements/plugin_dfElements.dir/Debug/*.obj")
set (TARGET_OBJ ${objFiles})

SET_SOURCE_FILES(
	test.cpp
)

SET_PRIVATE_HEADERS(
)

END_PROJECT()