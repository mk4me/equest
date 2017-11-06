BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Export.h
	WrapperT.h
	BufferPolicyT.h
	SQLiteBLOB.h
	SQLiteBLOBStreamBufferT.h
	SQLiteBLOBStreamBufferTImpl.h
	SQLiteDB.h
	SQLitePreparedStatement.h
)

SET_SOURCE_FILES(
	SQLiteBLOB.cpp	
	SQLiteDB.cpp
	SQLitePreparedStatement.cpp
)

END_PROJECT()