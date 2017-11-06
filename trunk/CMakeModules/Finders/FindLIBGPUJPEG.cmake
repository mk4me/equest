# przygotowanie do szukania
FIND_INIT(LIBGPUJPEG libgpujpeg)

# szukanie


if(WIN32)
	FIND_SHARED(LIBGPUJPEG "gpujpeg" "gpujpeg")
elseif(UNIX)
	FIND_SHARED(LIBGPUJPEG "libGPUJPEG" "libGPUJPEG")
endif()

# skopiowanie
FIND_FINISH(LIBGPUJPEG)

