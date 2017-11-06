# przygotowanie do szukania
FIND_INIT(DCMTK dcmtk)

FIND_DEPENDENCIES(DCMTK "ZLIB")

set(DCMTK_CONFIG_HEADERS "${includeDirRoot}" CACHE PATH "Location of dcmtk config headers.")
set(DCMTK_ADDITIONAL_INCLUDE_DIRS ${DICOM_CONFIG_HEADERS})
# szukanie
FIND_STATIC(DCMTK_CHARLS "charls" "charls")
FIND_STATIC(DCMTK_CMR "cmr" "cmr")
FIND_STATIC(DCMTK_IOD "dcmiod" "dcmiod")
FIND_STATIC(DCMTK_RT "dcmrt" "dcmrt")
FIND_STATIC(DCMTK_WLM "dcmwlm" "dcmwlm")
IF( NOT WIN32 )
	FIND_STATIC(DCMTK_I2D "libi2d" "libi2d")
ENDIF( WIN32 )

FIND_STATIC(DCMTK_JPEG8 "ijg8" "ijg8")
FIND_STATIC(DCMTK_JPEG12 "ijg12" "ijg12")
FIND_STATIC(DCMTK_JPEG16 "ijg16" "ijg16")
FIND_STATIC(DCMTK_JPEG "dcmjpeg" "dcmjpeg")
FIND_STATIC(DCMTK_JPLS "dcmjpls" "dcmjpls")

FIND_STATIC(DCMTK_IMAGE "dcmimage" "dcmimage")
FIND_STATIC(DCMTK_PSTAT "dcmpstat" "dcmpstat")
FIND_STATIC(DCMTK_IMGLE "dcmimgle" "dcmimgle")

FIND_STATIC(DCMTK_QRDB "dcmqrdb" "dcmqrdb")
FIND_STATIC(DCMTK_TLS "dcmtls" "dcmtls")
FIND_STATIC(DCMTK_NET "dcmnet" "dcmnet")
FIND_STATIC(DCMTK_SIGN "dcmdsig" "dcmdsig")
FIND_STATIC(DCMTK_SEG "dcmseg" "dcmseg")
FIND_STATIC(DCMTK_SR "dcmsr" "dcmsr")

FIND_STATIC(DCMTK_DATA "dcmdata" "dcmdata")
FIND_STATIC(DCMTK_OFLOG "oflog" "oflog")
FIND_STATIC(DCMTK_OFSTD "ofstd" "ofstd")
FIND_STATIC(DCMTK_FG "dcmfg" "dcmfg")


IF( WIN32 )
	# TODO : jest jakis lepszy sposob?
	set(_ALL_LIBS ${_ALL_LIBS} netapi32 wsock32 Ws2_32)
	set(_ALL_RELEASE_LIBS ${_ALL_RELEASE_LIBS} netapi32 wsock32 Ws2_32)
	set(_ALL_DEBUG_LIBS ${_ALL_DEBUG_LIBS} netapi32 wsock32 Ws2_32)
ENDIF( WIN32 )

# skopiowanie
FIND_FINISH(DCMTK)
