# przygotowanie do szukania
FIND_INIT(CURL curl)

# szukanie
FIND_SHARED(CURL "libcurl<d,?><_imp,?>" "libcurl")

if (WIN32)
	FIND_PREREQUISITES(CURL "OPENSSL")
elseif (UNIX)
	set(CURL_COMPILER_DEFINITIONS HAVE_SYS_SOCKET_H)
	FIND_DEPENDENCIES(CURL "OPENSSL")
endif()

# skopiowanie
FIND_FINISH(CURL)

if(UNIX)
	include (CheckIncludeFiles)
	CHECK_INCLUDE_FILES("sys/socket.h" HAVE_SYS_SOCKET_H)
	if(NOT HAVE_SYS_SOCKET_H)
		#message("CURL - sys/socket.h not found")
		#set(LIBRARY_CURL_FOUND 0)
	endif()
endif()
