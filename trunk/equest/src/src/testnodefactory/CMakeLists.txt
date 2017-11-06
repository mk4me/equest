BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	TestNodeFactory.h
	TestDFNodeProcessing.h
	TestDFNodeSource.h
	TestDFPin.h
	TestNode.h
)

SET_SOURCE_FILES (
	TestNodeFactory.cpp
	TestDFNodeProcessing.cpp
	TestDFNodeSource.cpp
	TestDFPin.cpp
	TestNode.cpp
)

END_PROJECT()