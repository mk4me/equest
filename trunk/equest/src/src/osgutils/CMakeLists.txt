BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	CustomPrimitivesFactory.h
	OsgSchemeDrawer.h
	PtrPolicyOSG.h
	ClonePolicyOSG.h
	Vec8d.h	
	DualQuat.h
	DualQuatUtils.h
	QuatUtils.h
)

SET_PRIVATE_HEADERS( PCH.h )

SET_SOURCE_FILES(
	CustomPrimitivesFactory.cpp
	OsgSchemeDrawer.cpp
	PCH.cpp
	DualQuat.cpp
	DualQuatUtils.cpp
	QuatUtils.cpp
)
SET_PRECOMPILED_HEADER(PCH.h PCH.cpp)

END_PROJECT()
