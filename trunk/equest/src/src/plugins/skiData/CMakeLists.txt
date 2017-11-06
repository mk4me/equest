BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	SkiData.h
	SkiMetadata.h
	SkiDataParser.h
	SkiMetadataParser.h
	DataSource.h
	SkiDataInitializerT.h
	DataSourceWidget.h
)

#------------------------------------------------------------------------------
SET_SOURCE_FILES(
	SkiDataParser.cpp
	SkiMetadataParser.cpp
	DataSource.cpp
	DataSourceWidget.cpp
	Plugin.cpp
)

SET_MOC_FILES(
	DataSourceWidget.h
)

SET_UI_FILES(
	DataSourceWidget.ui	
)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()
