set(TMP_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${ORIGINAL_CMAKE_MODULE_PATH})

find_package(PythonLibs)
set(LIBRARY_PYTHON_FOUND 0)

if(PYTHONLIBS_FOUND)
	set(LIBRARY_PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIRS})
	set(LIBRARY_PYTHON_RELEASE_LIBS PYTHON_LIBRARY)
	if(${PYTHON_DEBUG_LIBRARY})
		set(LIBRARY_PYTHON_DEBUG_LIBS PYTHON_DEBUG_LIBRARY)
		set(LIBRARY_PYTHON_LIBRARIES optimized ${PYTHON_LIBRARY} debug ${PYTHON_DEBUG_LIBRARY})
	else()
		set(LIBRARY_PYTHON_LIBRARIES ${PYTHON_LIBRARY})
	endif()
	set(LIBRARY_PYTHON_FOUND 1)
endif()

set(CMAKE_MODULE_PATH ${TMP_CMAKE_MODULE_PATH})