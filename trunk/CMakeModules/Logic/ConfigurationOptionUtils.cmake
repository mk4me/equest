###############################################################################

# Makro ustawiaj¹ce pewn¹ opcjê konfiguracji.
# Parametry:
#	name	Nazwa makra.
#	info	Tekstowa informacja o opcji.
#	default	ON / OFF
macro(_SETUP_CONFIG_OPTION name info)
	if (CONFIG_${name})
		_SETUP_INTERNAL_CACHE_VALUE(${name} 1 "${info}")		
	else()
		_SETUP_INTERNAL_CACHE_VALUE(${name} 0 "${info}")
	endif()
endmacro(_SETUP_CONFIG_OPTION)

###############################################################################

# Makro ustawiaj¹ce pewn¹ opcjê konfiguracji.
# Parametry:
#	name	Nazwa makra.
#	info	Tekstowa informacja o opcji.
#	default	ON / OFF
macro(CONFIG_OPTION name info default)
	option(CONFIG_${name} "${info}" "${default}")
	_SETUP_CONFIG_OPTION(${name} "${info}")
endmacro(CONFIG_OPTION)

# Makro pomocnicze neguj¹ce stan opcji
# Parametry:
#	state	Stan opcji - mo¿e przyjmowaæ wartoci ON lub OFF
# Wyjcie:
#	_stateNegation
macro(_NEGATE_OPTION_STATE state)

	set(_stateNegation)
	if(${state} STREQUAL "ON")
		set(_stateNegation "OFF")
	elseif(${state} STREQUAL "OFF")
		set(_stateNegation "ON")
	endif()
endmacro(_NEGATE_OPTION_STATE)
