# nazwa
set(TARGET_TARGETNAME
	vm_OSGPlugin
)

# Ÿród³a
set(TARGET_SRC 
	ReaderWriterVMPlugin.cpp
	VideoImageStream.cpp
	BufferedVideoImageStream.cpp
)

# nag³ówki
set(TARGET_H
	VideoImageStream.h
	BufferedVideoImageStream.h
)

# nag³ówki
set(INSTALL_H
	VideoImageStream.h
	BufferedVideoImageStream.h
)

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

# GET_TARGET_PROPERTY(AAAAA ${TARGET_TARGETNAME} LOCATION)
#message(${DEFAULT_PROJECT_LIBS} "  " ${CMAKE_INTDIR})

# jako plugin trzymamy go w podkatalogu
# set_target_properties(${TARGET_TARGETNAME} PROPERTIES PREFIX "osgPlugins-2.9.6/")                     
