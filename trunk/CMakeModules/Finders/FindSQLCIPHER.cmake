# przygotowanie do szukania
FIND_INIT(SQLCIPHER sqlcipher)

# szukanie
#if (WIN32) 
	FIND_SHARED(SQLCIPHER "sqlcipher" "sqlcipher")
	FIND_DEPENDENCIES(SQLCIPHER "OPENSSL")
#elseif (UNIX)
#	FIND_SHARED(SQLCIPHER "sqlcipher3" "sqlcipher3")
#	FIND_PREREQUISITES(SQLCIPHER "OPENSSL")
#endif()

set(SQLCIPHER_COMPILER_DEFINITIONS SQLITE_HAS_CODEC)
# skopiowanie
FIND_FINISH(SQLCIPHER)
