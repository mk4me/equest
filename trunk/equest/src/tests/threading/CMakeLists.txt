BEGIN_PROJECT("executable")

SET_SOURCE_FILES( 
	test.cpp
	SynchronizedTest.cpp
	ThreadTest.cpp
	ThreadPoolTest.cpp
	JobManagerTest.cpp
)

SET_PRIVATE_HEADERS(
	SynchronizedTest.h
	ThreadTest.h
	ThreadPoolTest.h	
	JobManagerTest.h
)

END_PROJECT()