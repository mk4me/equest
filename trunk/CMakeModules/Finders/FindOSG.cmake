# przygotowanie do szukania
FIND_INIT(OSG osg)

# funkcja wykrywaj¹ce wersjê OSG
function(OSG_FIND_VERSION path suffix)    
	# inicjalizacja
	set(OSG_VERSION${suffix} "Unknown" CACHE STRING "Unknown version")
	set(OSG_VERSION${suffix}_SO "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	if (EXISTS ${path})
		file(READ "${path}" _osg_Version_contents)
		string(REGEX REPLACE ".*#define [A-Z]+_MAJOR_VERSION[ \t]+([0-9]+).*"
            "\\1" _version_major ${_osg_Version_contents})
        string(REGEX REPLACE ".*#define [A-Z]+_MINOR_VERSION[ \t]+([0-9]+).*"
            "\\1" _version_minor ${_osg_Version_contents})
        string(REGEX REPLACE ".*#define [A-Z]+_PATCH_VERSION[ \t]+([0-9]+).*"
            "\\1" _version_patch ${_osg_Version_contents})
		string(REGEX REPLACE ".*#define [A-Z]+_SOVERSION[ \t]+([0-9]+).*"
            "\\1" _version_so ${_osg_Version_contents})	
		if (OSG_VERSION${suffix} STREQUAL "Unknown")
			set(OSG_VERSION${suffix} "${_version_major}.${_version_minor}.${_version_patch}" CACHE STRING "The version of OSG_VERSION${suffix} which was detected" FORCE)
		endif()
		if (OSG_VERSION${suffix}_SO STREQUAL "Unknown")
			set(OSG_VERSION${suffix}_SO "${_version_so}" CACHE STRING "The version of OSG_VERSION${suffix}_SO which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (OSG_VERSION${suffix} STREQUAL "Unknown")
		FIND_NOTIFY(OSG_VERSION${suffix} "Unknown version of OSG_VERSION${suffix}. File ${path} could not be read. This may result in further errors.")
	endif()
	if (OSG_VERSION${suffix}_SO STREQUAL "Unknown")
		FIND_NOTIFY(OSG_VERSION${suffix}_SO "Unknown interface version of OSG_VERSION${suffix}_SO. File ${path} could not be read. This may result in further errors.")
	endif()
endfunction(OSG_FIND_VERSION)

# wykrycie wersji osg
OSG_FIND_VERSION("${OSG_INCLUDE_DIR}/osg/Version"  "")

# OSG
FIND_SHARED(OSG_LIBCORE osg "osg${OSG_VERSION_SO}-osg")
FIND_SHARED(OSG_LIBDB osgDB "osg${OSG_VERSION_SO}-osgDB")
FIND_SHARED(OSG_LIBUTIL osgUtil "osg${OSG_VERSION_SO}-osgUtil")
FIND_SHARED(OSG_LIBGA osgGA "osg${OSG_VERSION_SO}-osgGA")
FIND_SHARED(OSG_LIBVIEWER osgViewer "osg${OSG_VERSION_SO}-osgViewer")
FIND_SHARED(OSG_LIBTEXT osgText "osg${OSG_VERSION_SO}-osgText")
FIND_SHARED(OSG_LIBWIDGET osgWidget "osg${OSG_VERSION_SO}-osgWidget")
FIND_SHARED(OSG_LIBANIMATION osgAnimation "osg${OSG_VERSION_SO}-osgAnimation")
FIND_SHARED(OSG_LIBQT osgQt "osg${OSG_VERSION_SO}-osgQt")
FIND_SHARED(OSG_MANIPULATOR osgManipulator "osg${OSG_VERSION_SO}-osgManipulator")

FIND_DIRECTORY(OSG_PLUGINS "osgPlugins-${OSG_VERSION}")

FIND_DEPENDENCIES(OSG "OPENTHREADS")
FIND_PREREQUISITES(OSG "LIBPNG;GIFLIB;LIBTIFF;QT;OPENSSL;SQLITE;GLUT;CURL;ZLIB")

# skopiowanie
FIND_FINISH(OSG)