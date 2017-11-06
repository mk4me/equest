# przygotowanie do szukania
FIND_INIT(WSDLPULL wsdlpull)

# szukanie
FIND_SHARED(WSDLPULL "wsdlpull" "wsdlpull")

if(WIN32)
	FIND_DEPENDENCIES(WSDLPULL "CURL")
elseif(UNIX)
	FIND_PREREQUISITES(WSDLPULL "CURL")
endif()

# skopiowanie
FIND_FINISH(WSDLPULL)

set(WSDLPULL_COMPILER_DEFINITIONS WITH_CURL)