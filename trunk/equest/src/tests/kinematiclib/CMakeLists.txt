BEGIN_PROJECT("executable")

SET_SOURCE_FILES(
	test.cpp
	JointTest.cpp
	QuaternionManipulatorsTest.cpp
	SkeletalDataTest.cpp
	SkeletonTest.cpp
	SkeletonStateTest.cpp
	Viewer.cpp
)

SET_PRIVATE_HEADERS(
	JointTest.h
	QuaternionManipulatorsTest.h
	SkeletalDataTest.h
	SkeletonTest.h
	SkeletonStateTest.h
	Viewer.h
)


#TODO moze warto to jakos uniwersalnie zrobic..
if (UNIX)
	file(COPY "testFiles" DESTINATION "${CMAKE_BINARY_DIR}/bin")
else()
	file(COPY "testFiles" DESTINATION "${CMAKE_BINARY_DIR}/tests/kinematiclib")
endif()
END_PROJECT()
