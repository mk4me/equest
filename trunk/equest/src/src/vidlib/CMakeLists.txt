CONFIG_OPTION(VIDLIB_LIBRARY_STATIC "Build vidlib static library?" OFF)

if(VIDLIB_LIBRARY_STATIC)
	BEGIN_PROJECT("static")
else()
	BEGIN_PROJECT("dynamic")
endif()

CONFIG_OPTION(VIDLIB_ENABLE_EXCEPTIONS "Enable exeptions?" ON)

#------------------------------------------------------------------------------

SET_PRIVATE_HEADERS(
	Converter.h
	VideoBufferChunk.h
	VidLibPrivate.h
	PCH.h
)

set (PUBLIC 
	BufferedVideoStream.h
	Picture.h
	PixelFormat.h
	VideoError.h
	VideoStream.h
	VideoBuffer.h
	FFmpegError.h
	FFmpegVideoStream.h
	FileSequenceVideoStream.h
	VideoStreamAdapter.h
	)
	
set(SOURCE
	BufferedVideoStream.cpp
	Converter.cpp
	FFmpegError.cpp
	FFmpegVideoStream.cpp
	Picture.cpp
	VideoBuffer.cpp
	VideoBufferChunk.cpp
	VideoStream.cpp
	VideoError.cpp
	FileSequenceVideoStream.cpp
	VideoStreamAdapter.cpp
	PCH.cpp
	)

if (CONFIG_VIDLIB_ENABLE_OSG)
#	OSG
	set(PUBLIC ${PUBLIC}
		osg/BufferedVideoImageStream.h
		osg/VideoImageStream.h
		osg/VideoImageStreamSizeOptimizer.h
		osg/GenericImageStream.h
		osg/ReaderWriterVidLib.h
		osg/FFmpegImageStream.h
		osg/VideoImage.h
		osg/OsgAdapter.h
	)
	
	set(SOURCE ${SOURCE}
		osg/BufferedVideoImageStream.cpp
		osg/ReaderWriterVidLib.cpp
		osg/VideoImageStream.cpp
		osg/VideoImageStreamSizeOptimizer.cpp
		osg/FFmpegImageStream.cpp
		osg/VideoImage.cpp
		osg/OsgAdapter.cpp
	)
endif()

SET_PUBLIC_HEADERS(
	${PUBLIC}
)

SET_SOURCE_FILES(
	${SOURCE}
)

SET_CONFIGURATION_INPUT_FILES(
	Config.h.in
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")

SEARCH_DEPENDENCY_CONFIGURATION_DEFINES("Config.h" "VIDLIB_ENABLE_LOG4CXX;VIDLIB_ENABLE_OSG")

SET_PRECOMPILED_HEADER( PCH.h PCH.cpp)

END_PROJECT()
