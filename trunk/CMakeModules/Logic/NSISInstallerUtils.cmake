###############################################################################
# Makro dodaje skroty
# Parametry:
#		variable - zmienna jakiej ustawie komendy do zmiany kontekstu uzytkownika podczas instalacji
#		userContext - kontekst uzytkownika: currentUser | all | admin
macro(NSIS_SWITCH_USER_CONTEXT variable userContext)
	
	if(${userContext} STREQUAL "currentUser")
		set(${variable} "SetShellVarContext current")
	elseif(${userContext} STREQUAL "all")
		set(${variable} "SetShellVarContext all")
	elseif(${userContext} STREQUAL "admin")
		set(${variable} "SetShellVarContext admin")
	else()
		set(${variable} "")
		INSTALLER_NOTIFY(${variable} "Unrecognized NSIS user context ${userContext} - should be one of: currentUser, all, admin. Skipping user context.")
	endif()
	
endmacro(NSIS_SWITCH_USER_CONTEXT)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		path - sciezka gdzie utworzony zostanie link
#		object - sciezka obiektu w ramach instalacji do ktorego tworzymy link
#		icon - ikona dla skrotu
#		[iconIndex] - index ikony skrotu
#		[startOptions] - opcje startowe
#		[keyboardShortcut] - skrót klawiszowy
#		[description] - krótki opis
macro(NSIS_INSTALLER_SHORTCUT_EXT installVariable uninstalVariable path object icon)

	# TODO
	# obsluzyc wszystkie opcje skrótów	
	# https://github.com/NSIS-Dev/Documentation/blob/master/Reference/CreateShortCut.md
	
	set(${variable} "CreateShortCut '${path}' '${object}' '' '${icon}'")
	set(${uninstalVariable} "Delete '${path}'")
	
endmacro(NSIS_INSTALLER_SHORTCUT_EXT)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		variable - tutaj ustawie wygenerowana sciezke
#		path - sciezka pliku wzgledem pulpitu
macro(NSIS_DESKTOP_PATH variable path)
	
	set(${variable} "$DESKTOP\${path}")
	
endmacro(NSIS_DESKTOP_PATH)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		variable - tutaj ustawie wygenerowana sciezke
#		path - sciezka pliku wzgledem pulpitu
macro(NSIS_STARTMENU_PATH variable path)
	
	set(${variable} "$SMPROGRAMS\${path}")
	
endmacro(NSIS_STARTMENU_PATH)

###############################################################################
# Makro dodaje skroty
# Parametry:
#		variable - tutaj ustawie wygenerowana sciezke
#		path - sciezka pliku wzgledem pulpitu
macro(NSIS_CONVERT_PATH variable path)

	#string(REPLACE "//" "/" ${variable} ${path})	
	string(REPLACE "/" "\\\\" ${variable} ${path})
	
endmacro(NSIS_CONVERT_PATH)

###############################################################################
# Makro instaluje zasoby komponentow ktore musza byc edytowalne dla aplikacji
# Parametry:
#		resPath - sciezka zasobow w drzewie SCM (svn)
#		[args] - lista zasobow do instalacji
macro(_INSTALL_NSIS_MODIFYABLE_RESOURCES resPath installPath)

	foreach(r ${ARGN})				
		# Komendy NSIS dla modyfikowalnych zasobów aplikacji
		get_filename_component(_path "${r}" PATH)		
		get_filename_component(_fileName "${r}" NAME)
		
		set(_directory "$APPDATA/${installPath}/resources/${_path}")
		set(_file "${resPath}/${r}")
		set(_fullPath "$APPDATA/${installPath}/resources/${r}")
		
		#musimy poprawić ścieżki dla NSISA
		NSIS_CONVERT_PATH(_directory "${_directory}")
		NSIS_CONVERT_PATH(_file "${_file}")
		NSIS_CONVERT_PATH(_fullPath "${_fullPath}")
		
		# UWAGA - te komendy wykonują się dla wszystkich użytkowników,
		# jeśli chemy robić cos dla aktualnego użytkownika musimy tu jeszcze wpleść komendę :
		# SetShellVarContext current | admin | all
		# przed odpowiednimu wywołaniami
		# teraz jest ok bo mamy tylko log.ini z core dla log4cxx z konfiguracją
	
		set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
			CreateDirectory \\\"${_directory}\\\"
			SetOutPath \\\"${_directory}\\\"
			File \\\"${_file}\\\"" CACHE INTERNAL "Dodatkowe komendy instalacji" FORCE)
			
		set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
			Delete \\\"${_fullPath}\\\"" CACHE INTERNAL "Dodatkowe komendy deinstalacji" FORCE)
	endforeach()

endmacro(_INSTALL_NSIS_MODIFYABLE_RESOURCES)