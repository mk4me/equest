# nazwa
set(TARGET_TARGETNAME
	vm_core
)

# Ÿród³a
set(TARGET_SRC 
	BufferedVideoStream.cpp
	Converter.cpp
	FFmpegVideoStream.cpp
	Picture.cpp
	VideoBuffer.cpp
	VideoBufferChunk.cpp
	VideoStream.cpp
	VideoManager.cpp
	PixelFormat.cpp
)

# nag³ówki
set(TARGET_H
	VM.h
	VMPrivate.h
	BufferedVideoStream.h
	Converter.h
	FFmpegError.h
	FFmpegVideoStream.h
	Picture.h
	PixelFormat.h
	Utils.h
	VideoError.h
	VideoInput.h
	VideoStream.h
	VideoBuffer.h
	VideoBufferChunk.h
	VideoManager.h
)

# nag³ówki
set(INSTALL_H
	VM.h
	BufferedVideoStream.h
	Converter.h
	Picture.h
	PixelFormat.h
	Utils.h
	VideoError.h
	VideoInput.h
	VideoStream.h
)

if(CMAKE_COMPILER_IS_GNUCXX)
    # Remove -pedantic flag as it barfs on ffmpeg headers
   string(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})

# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})

# instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${INSTALL_H} DESTINATION include/${TARGET_TARGETNAME})
install(FILES "${PROJECT_BINARY_DIR}/${TARGET_TARGETNAME}/Config.h" DESTINATION include/${TARGET_TARGETNAME})

# Kopiujemy konfiguracja
configure_file( "Config.h.in" "${PROJECT_BINARY_DIR}/core/Config.h" )

#message("${PROJECT_BINARY_DIR}/${TARGET_TARGETNAME}/Config.h")

