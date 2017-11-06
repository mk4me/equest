BEGIN_PROJECT("executable")


FILE(GLOB objFiles  "${CMAKE_BINARY_DIR}/src/plugins/newVdf/plugin_newVdf.dir/Debug/*.obj")

set (TARGET_OBJ ${objFiles})

SET_SOURCE_FILES(
	test.cpp
	MergeTest.cpp
    SaveSceneTest.cpp
	UniversalSourceTest.cpp
)

SET_PRIVATE_HEADERS(
	MergeTest.h
    SaveSceneTest.h
	UniversalSourceTest.h
)

END_PROJECT()