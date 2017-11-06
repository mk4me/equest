# przygotowanie do szukania
set(FIND_DISABLE_INCLUDES ON)
FIND_INIT(BTK btk)

FIND_SHARED_EXT(BTK_IO "BTKIO" "BTKIO" "BTKIO" "BTKIO")
FIND_SHARED_EXT(BTK_Common "BTKCommon" "BTKCommon" "BTKCommon" "BTKCommon")
FIND_SHARED_EXT(BTK_Filters "BTKBasicFilters" "BTKBasicFilters" "BTKBasicFilters" "BTKBasicFilters")

FIND_DEPENDENCIES(BTK "EIGEN3")

# koniec
FIND_FINISH(BTK)

set(BTK_COMPILER_DEFINITIONS EIGEN2_SUPPORT)

if(LIBRARY_BTK_FOUND)	
	# szukanie
	set(BTK_Config_DIR "${FIND_LIBRARIES_INCLUDE_ROOT}/btk/" CACHE PATH "Location of BTK config headers.")
	set(BTK_IO_INCLUDE_DIR "${BTK_Config_DIR}/IO" CACHE PATH "Location of BTK IO headers.")
	set(BTK_Common_INCLUDE_DIR "${BTK_Config_DIR}/Common" CACHE PATH "Location of BTK Common headers.")
	set(BTK_Filters_INCLUDE_DIR "${BTK_Config_DIR}/BasicFilters" CACHE PATH "Location of BTK Filters headers.")	

	set(BTK_ADDITIONAL_INCLUDE_DIRS 
		"${BTK_ADDITIONAL_INCLUDE_DIRS};${BTK_IO_INCLUDE_DIR};${BTK_Common_INCLUDE_DIR};${BTK_Filters_INCLUDE_DIR};${BTK_Config_DIR}"
		CACHE PATH "Location of BTK include headers.")
	
endif()
