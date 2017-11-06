macro(BEGIN_PLUGIN_PROJECT type)
	BEGIN_PROJECT(${type})
endmacro(BEGIN_PLUGIN_PROJECT)

macro(END_PLUGIN_PROJECT)
	END_PROJECT()
	GENERATE_PLUGIN_ARTIFACTS()
endmacro(END_PLUGIN_PROJECT)

#---------------------------------------------------
# makro pomagające logować stan konfiguracji

macro(VERBOSE_MESSAGE var msg)
	if (CONFIG_SOLUTION_VERBOSE)
		message(STATUS "EXTERNAL>${var}>${msg}")
	endif()
endmacro(VERBOSE_MESSAGE)

###############################################################################
function(procedural_create_vcproj_userfile TARGETNAME WORKING_DIR EXE_FILENAME)
  if (MSVC)
	set(PROCEDURAL_TEMPLATES_DIR "../../../../CMakeModules/Templates")
	if (${MSVC_VERSION} LESS 1600)
	  configure_file(
	  ${PROCEDURAL_TEMPLATES_DIR}/VisualStudioUserFile.vcproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
	  @ONLY
	)
	else()
	  configure_file(
	  ${PROCEDURAL_TEMPLATES_DIR}/VisualStudioUserFile.vcxproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
	  @ONLY
	)
	endif()
  endif ()
 endfunction(procedural_create_vcproj_userfile)
 

###############################################################################
macro(GENERATE_UNIX_SCRIPT filepath exeCommand)
        if (UNIX)
		set (scriptT "\#!/bin/sh\nexport LD_LIBRARY_PATH=")
		  
		foreach(value ${ALL_LIBRARIES})
			set(dir "${${value}_LIBRARY_DIR_RELEASE}")
			if (DEFINED ${value}_LIBRARY_DIR_RELEASE)
				set (scriptT "${scriptT}${dir}:")
			else()
				VERBOSE_MESSAGE(GENERATE_UNIX_SCRIPT "nie ma sciezki do biblioteki ${value} : nie bedzie dodana do skryptu uruchamiajacego")
			endif()
		endforeach()

		set( scriptT "${scriptT}:$LD_LIBRARY_PATH\n")

		list (FIND ALL_LIBRARIES "QT" _index)
		if (${_index} GREATER -1)
			set(qtdir "${QT_LIBRARY_DIR_RELEASE}")
			if (DEFINED QT_LIBRARY_DIR_RELEASE)

			set (scriptT "${scriptT}export QT_QPA_PLATFORM_PLUGIN_PATH=${qtdir}/plugins\n")
			set (scriptT "${scriptT}export QT_QPA_FONTDIR=${qtdir}/fonts\n")
			endif()
		endif()
		
		set( script_run "${scriptT}exec ${exeCommand} $*")
		set( filename_run "${filepath}")
		file (WRITE ${filename_run} "${script_run}")
		execute_process(COMMAND chmod a+x ${filename_run})
	endif()
endmacro() 

###############################################################################
# Makro generuje wykonywalny skrypt z ustawionymi sciezkami do bibliotek
macro(GENERATE_UNIX_EXECUTABLE_SCRIPT)
	if (UNIX)
		GENERATE_UNIX_SCRIPT(
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_${TARGET_TARGETNAME}.sh"
			"./${TARGET_TARGETNAME}"
		)

		GENERATE_UNIX_SCRIPT(
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gdb_${TARGET_TARGETNAME}.sh"
			"gdb ./${TARGET_TARGETNAME}"
		)
		#set (scriptT "\#!/bin/sh\nexport LD_LIBRARY_PATH=")
		  
		#foreach(value ${ALL_LIBRARIES})
		#	set(dir "${${value}_LIBRARY_DIR_RELEASE}")
		#	set (scriptT "${scriptT}${dir}:")
		#endforeach()

		#set( scriptT "${scriptT}:$LD_LIBRARY_PATH\n")

		#set( script_run "${scriptT}exec ./${TARGET_TARGETNAME} $*")
		#set( filename_run "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_${TARGET_TARGETNAME}.sh")
		#file (WRITE ${filename_run} "${script_run}")
		#execute_process(COMMAND chmod a+x ${filename_run})

		#set( script_debug "${scriptT}exec gdb ./${TARGET_TARGETNAME} $*")
		#set( filename_debug "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/gdb_${TARGET_TARGETNAME}.sh")
		#file (WRITE ${filename_debug} "${script_debug}")
		#execute_process(COMMAND chmod a+x ${filename_debug})
	endif()
endmacro(GENERATE_UNIX_EXECUTABLE_SCRIPT)



###############################################################################
# Makro generuje wykonywalny skrypt z ustawionymi sciezkami do bibliotek
macro(GENERATE_UNIX_PlUGIN_STARTER_SCRIPT)
	if (UNIX)
		GENERATE_UNIX_SCRIPT(
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_${TARGET_TARGETNAME}.sh"
			"${MDE_CORE_PATH}/${MDE_CORE_EXE} --plugins ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../lib"
		)
		#set (scriptT "\#!/bin/sh\nexport LD_LIBRARY_PATH=")
		  
		#foreach(value ${ALL_LIBRARIES})
		#	set(dir "${${value}_LIBRARY_DIR_RELEASE}")
		#	if (DEFINED ${value}_LIBRARY_DIR_RELEASE)
		#		set (scriptT "${scriptT}${dir}:")
		#	else()
		#		message("nie ma sciezki do biblioteki ${value} : nie bedzie dodana do skryptu uruchamiajacego")
		#	endif()
		#endforeach()

		#set( scriptT "${scriptT}:$LD_LIBRARY_PATH\n")

		#set( script_run "${scriptT}exec ${PROCEDURAL_CORE_PATH}/${PROCEDURAL_CORE_EXE} --plugins ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../lib $*")
		#set( filename_run "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_${TARGET_TARGETNAME}.sh")
		#file (WRITE ${filename_run} "${script_run}")
		#execute_process(COMMAND chmod a+x ${filename_run})
	endif()
endmacro(GENERATE_UNIX_PlUGIN_STARTER_SCRIPT)

macro(GENERATE_PLUGIN_ARTIFACTS)
	if (WIN32)
		GET_FILENAME_COMPONENT(PLUGINS_DESTINATION_DIR_TMP  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PJWSTK\\EDR;ApplicationDataPath]" ABSOLUTE CACHE)
		file(TO_NATIVE_PATH ${PLUGINS_DESTINATION_DIR_TMP} PLUGINS_DESTINATION_DIR_NATIVE)
		
		add_custom_command(  TARGET ${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} 
							 POST_BUILD 
							 COMMAND mkdir \"${4}\\plugins\" | mkdir \"${PLUGINS_DESTINATION_DIR_NATIVE}\\plugins\\${TARGET_TARGETNAME}\" | copy \"$(SolutionDir)bin\\$(Configuration)\\$(TargetName)$(TargetExt)\" \"${PLUGINS_DESTINATION_DIR_NATIVE}\\plugins\\${TARGET_TARGETNAME}\\$(TargetName)$(TargetExt)\" )
		procedural_create_vcproj_userfile(${PROJECT_${CURRENT_PROJECT_NAME}_TARGETNAME} ${MDE_CORE_PATH} ${MDE_CORE_EXE})
	elseif(UNIX)
		GENERATE_UNIX_PlUGIN_STARTER_SCRIPT()
	endif()
endmacro(GENERATE_PLUGIN_ARTIFACTS)


###############################################################################

# Inicjuje konfigurację solucji
# Parametry:
	# [zewnętrzne biblioteki potrzebne w solucji]
	# [dodatkowe katalogi dla CMakeModules poza CustomCMakeModules - ten jest automatycznie dodawany]
	# [dodatkowe definicje preprocesora dla całej solucji]
	# [dodatkowe flagi kompilatora dla całej solucji]
macro(INITIALIZE_PLUGINS_SOLUTION name)

INITIALIZE_SOLUTION("${name}")
	
if (WIN32)
	GET_FILENAME_COMPONENT(PROGRAM_FILES_DIR_TMP  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PJWSTK\\EDR;ProgramFilesPath]" ABSOLUTE CACHE)
	file(TO_NATIVE_PATH ${PROGRAM_FILES_DIR_TMP} PROGRAM_FILES_DIR)
	set(MDE_CORE_PATH ${PROGRAM_FILES_DIR}/bin CACHE PATH "Binary path.")
	set(MDE_CORE_EXE core.exe CACHE STRING "Executable name")
else()
	set(MDE_CORE_PATH ${PROJECT_BINARY_DIR}/../edr/bin CACHE PATH "Binary path.")
	set(MDE_CORE_EXE old_view CACHE STRING "Executable name")
endif()
endmacro(INITIALIZE_PLUGINS_SOLUTION)

#---------------------------------------------------
# makro kończące konfigurację solucji
macro(FINALIZE_PLUGINS_SOLUTION)
	FINALIZE_SOLUTION()
	if (WIN32)
		set(temp_path ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
		GET_FILENAME_COMPONENT(PLUGINS_DEST  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\PJWSTK\\EDR;ApplicationDataPath]" ABSOLUTE CACHE)
		# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PLUGINS_DESTINATION_DIR_TMP}/plugins/libs")
		# #message ("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
		# FIND_HANDLE_MODULES(PROJECT_COPY_MODULES)
		# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${temp_path})
	endif()
endmacro(FINALIZE_PLUGINS_SOLUTION)



