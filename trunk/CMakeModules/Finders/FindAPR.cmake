FIND_INIT(APR apr)

# szukanie
FIND_SHARED(APR "libapr<-1,?>" "libapr<-1,?>")

# skopiowanie
FIND_FINISH(APR)

if(WIN32)
	FIND_NOTIFY(APR "Library APR is dedicated for linux platform. Are you sure you need it for windows?")
endif()
