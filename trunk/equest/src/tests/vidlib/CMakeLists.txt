BEGIN_PROJECT("executable")

SET_SOURCE_FILES( 
	test.cpp
	FFmpegVideoStreamTest.cpp
	OsgAdapterTest.cpp
)

SET_PRIVATE_HEADERS(
	FFmpegVideoStreamTest.h
)

END_PROJECT()