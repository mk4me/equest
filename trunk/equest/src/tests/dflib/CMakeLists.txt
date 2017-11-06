BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	ModelOperationsTest.h
	IntPin.h
	IntSource.h
	IntProcessor.h
	IntSink.h
)

SET_SOURCE_FILES(
	main.cpp
	ModelOperationsTest.cpp
)

END_PROJECT()