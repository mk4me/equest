BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Export.h
	SubjectDataFilters.h
	ISubject.h
	ISession.h
	IMotion.h
	Types.h
	ISubjectService.h
	IntegumentarySystem.h
	ISubjectSystem.h
	MuscularSystem.h
	SkeletalSystem.h
	SolidSystem.h
	SubjectSystemConfiguration.h
)

SET_PRIVATE_HEADERS(
	BasicDataStorage.h
	SubjectService.h
	Motion.h
	Session.h
	Subject.h
	SubjectPCH.h
)

SET_SOURCE_FILES (
	BasicDataStorage.cpp
	Motion.cpp
	Session.cpp
	Subject.cpp
	SubjectService.cpp
	SubjectPlugin.cpp
	SubjectPCH.cpp
	SubjectDataFilters.cpp
)

SET_PRECOMPILED_HEADER( SubjectPCH.h SubjectPCH.cpp)

END_PROJECT()