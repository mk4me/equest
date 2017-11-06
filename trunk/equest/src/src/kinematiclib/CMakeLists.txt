BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(	
	SkeletonMappingScheme.h
	QuaternionManipulators.h
	SkeletalData.h
	Skeleton.h
	SkeletonState.h
	SkeletonStateChange.h
	Topology.h
	LinearizedSkeleton.h
	Types.h
)	

SET_SOURCE_FILES( 	
	SkeletonMappingScheme.cpp
	Skeleton.cpp
	SkeletonState.cpp
	SkeletonStateChange.cpp
	LinearizedSkeleton.cpp
)

END_PROJECT()