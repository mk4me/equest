BEGIN_PROJECT("executable")

SET_SOURCE_FILES( 
	test.cpp
	ThreadPoolTest.cpp
	JobManagerTest.cpp
	ThreadTest.cpp
)

SET_PRIVATE_HEADERS(
	ThreadPoolTest.h	
	JobManagerTest.h
	ThreadTest.h
	TestObject.h
)

END_PROJECT()