BEGIN_PROJECT("executable")


FILE(GLOB objFiles  "${CMAKE_BINARY_DIR}/src/plugins/dicom/plugin_dicom.dir/Debug/*.obj")

set (TARGET_OBJ ${objFiles})

SET_SOURCE_FILES(
	test.cpp
	LayeredImageTest.cpp
)

SET_PRIVATE_HEADERS(
	LayeredImageTest.h
)

END_PROJECT()