BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Export.h
	ShallowCopy.h
	IAccountFactoryWS.h
	AccountFactoryWS.h
	IAdministrationWS.h
	AdministrationWS.h
	IAuthorizationWS.h
	AuthorizationWS.h
	IBasicQueriesWS.h
	BasicQueriesWS.h
	IBasicUpdatesWS.h
	BasicUpdatesWS.h
	IFileStoremanWS.h
	FileStoremanWS.h
	IUserPersonalSpaceWS.h
	UserPersonalSpaceWS.h
	MetadataParser.h
	ShallowCopyParser.h
	IncrementalBranchShallowCopy.h
	IncrementalBranchShallowCopyParser.h
	DateTime.h
	DateTimeUtils.h
	Entity.h
)

SET_PRIVATE_HEADERS(
	XMLHelper.h
)

SET_SOURCE_FILES(
	AccountFactoryWS.cpp
	AdministrationWS.cpp
	AuthorizationWS.cpp
	BasicUpdatesWS.cpp
	BasicQueriesWS.cpp
	FileStoremanWS.cpp	
	UserPersonalSpaceWS.cpp
	MetadataParser.cpp
	ShallowCopyParser.cpp
	IncrementalBranchShallowCopyParser.cpp
	DateTime.cpp
	DateTimeUtils.cpp
	Entity.cpp
	XMLHelper.cpp
)

END_PROJECT()