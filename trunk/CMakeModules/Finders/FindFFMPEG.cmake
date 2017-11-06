# przygotowanie do szukania
FIND_INIT(FFMPEG ffmpeg)

# szukanie
# funkcja wykrywaj¹ce wersjê OPEN CV
function(FFMPEG_FIND_MODULE_VERSION path lowerModule upperModule)    
	set(version_path "${path}/lib${lowerModule}/version.h")
	# inicjalizacja
	set(FFMPEG_LIB${upperModule}_VERSION "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	
	if (EXISTS "${version_path}")
		file(READ "${version_path}" _ffmpeg_Version_contents)
		string(REGEX REPLACE ".*#define LIB${upperModule}_VERSION_MAJOR[ \t]+([0-9]+).*"
            "\\1" _version_major ${_ffmpeg_Version_contents})
		if (FFMPEG_LIB${upperModule}_VERSION STREQUAL "Unknown")
			set(FFMPEG_LIB${upperModule}_VERSION "${_version_major}" CACHE STRING "The version of FFMPEG_LIB${upperModule}_VERSION which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (FFMPEG_LIB${upperModule}_VERSION STREQUAL "Unknown")
		FIND_NOTIFY(FFMPEG_LIB${upperModule}_VERSION "Unknown version of FFMPEG_LIB${upperModule}_VERSION. File ${version_path} could not be read. This may result in further errors.")
	endif()
endfunction(FFMPEG_FIND_MODULE_VERSION)

macro(FFMPEG_FIND_SHARED module)

string(TOUPPER "${module}" upperModule)
string(TOLOWER "${module}" lowerModule)

FFMPEG_FIND_MODULE_VERSION("${FFMPEG_INCLUDE_DIR}" "${lowerModule}" "${upperModule}")
FIND_SHARED(FFMPEG_LIB${upperModule} "${lowerModule}<-${FFMPEG_LIB${upperModule}_VERSION},?>" "${lowerModule}<-${FFMPEG_LIB${upperModule}_VERSION},?>")

endmacro(FFMPEG_FIND_SHARED)

FFMPEG_FIND_SHARED(avcodec)
FFMPEG_FIND_SHARED(avdevice)
FFMPEG_FIND_SHARED(avfilter)
FFMPEG_FIND_SHARED(avformat)
FFMPEG_FIND_SHARED(avutil)
FFMPEG_FIND_SHARED(swresample)
FFMPEG_FIND_SHARED(swscale)

#FIND_SHARED(FFMPEG_LIBAVCODEC "avcodec<-55,?>" "avcodec-55")
#FIND_SHARED(FFMPEG_LIBAVDEVICE "avdevice<-55,?>" "avdevice-55")
#FIND_SHARED(FFMPEG_LIBAVFILTER "avfilter<-4,?>" "avfilter-4")
#FIND_SHARED(FFMPEG_LIBAVFORMAT "avformat<-55,?>" "avformat-55")
#FIND_SHARED(FFMPEG_LIBAVUTIL "avutil<-52,?>" "avutil-52")
#FIND_SHARED(FFMPEG_LIBSWRESAMPLE "swresample<-0,?>" "swresample-0")
#FIND_SHARED(FFMPEG_LIBSWSCALE "swscale<-2,?>" "swscale-2")

if(UNIX)
	FIND_DEPENDENCIES(FFMPEG "OPENSSL;FREETYPE")
endif()

# skopiowanie
FIND_FINISH(FFMPEG)