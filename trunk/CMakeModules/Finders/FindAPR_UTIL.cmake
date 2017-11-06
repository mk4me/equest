FIND_INIT(APR_UTIL apr-util)

# szukanie
FIND_SHARED(APR_UTIL "libaprutil<-1,?>" "libaprutil<-1,?>")

# skopiowanie
FIND_FINISH(APR_UTIL)

if(WIN32)
	FIND_NOTIFY(APR "Library APR is dedicated for linux platform. Are you sure you need it for windows?")
endif()
