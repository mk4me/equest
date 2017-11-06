BEGIN_PROJECT("executable")

SET_SOURCE_FILES( 
	test.cpp
	AlignTest.cpp
	PoolTest.cpp
	UtilsTest.cpp
	EnumTest.cpp
	ObserverPatternTest.cpp
	NamedTreeBaseTest.cpp
	DataChannelTest.cpp
)

SET_PRIVATE_HEADERS(
	AlignTest.h
	PoolTest.h
	UtilsTest.h
	EnumTest.h
	ObserverPatternTest.h
	NamedTreeBaseTest.h
	DataChannelTest.h
)

END_PROJECT()