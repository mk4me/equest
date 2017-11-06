BEGIN_PROJECT("executable")

SET_SOURCE_FILES(
	test.cpp
	AsfParserTest.cpp
	AmcParserTest.cpp
)

SET_PRIVATE_HEADERS(
	AsfParserTest.h
	AmcParserTest.h
)
#TODO moze warto to jakos uniwersalnie zrobic..
file(COPY "testFiles" DESTINATION "${CMAKE_BINARY_DIR}/tests/acclaimFormats")

END_PROJECT()