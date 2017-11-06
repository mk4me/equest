###############################################################################

# Makro pomocne do definiowania różnych gałęzi logowania

macro(INIT_VERBOSE_OPTION_EXT name comment defaultValue)

	CONFIG_OPTION(${name}_VERBOSE "${comment}" ${defaultValue})

endmacro(INIT_VERBOSE_OPTION_EXT)

###############################################################################

# Makro pomocne do definiowania różnych gałęzi logowania

macro(INIT_VERBOSE_OPTION name comment)

	INIT_VERBOSE_OPTION_EXT("${name}" "${comment}" OFF)

endmacro(INIT_VERBOSE_OPTION)

###############################################################################

macro(STRING_FIND input text output_index forward)

	if(${forward})
		string(FIND "${input}" "${text}" ${output_index} )
	else()
		string(FIND "${input}" "${text}" ${output_index} REVERSE)
	endif()

endmacro(STRING_FIND)

###############################################################################

macro(_STRING_SUBSTRING input output globalIDX idx length forward)
	
	if(${forward})
		math(EXPR ${globalIDX} "${${globalIDX}}+${idx}")
		string(LENGTH "${input}" _ilength)
		math(EXPR _idx "${idx} + ${length}")
		math(EXPR _ilength "${_ilength} - ${_idx}")		
		string(SUBSTRING "${input}" ${_idx} ${_ilength} _output)
	else()
		set(${globalIDX} ${idx})
		string(SUBSTRING "${input}" 0 ${idx} _output)
	endif()
	set(${output} "${_output}")

endmacro(_STRING_SUBSTRING)

###############################################################################

# Makro do podmiany w stringu tylko wybranego pasującego z lewej lub prawej ciągu znaków

macro(STRING_REPLACE_ONE input replace text output index forward)

	if(${index} GREATER -1)
		math(EXPR _index "${index}+1")
		set(_count 0)
		set(_tmpIDX -1)
		set(_cIDX 0)
		set(_input "${input}")
		string(LENGTH "${replace}" rLength)
		STRING_FIND("${_input}" "${replace}" _tmpIDX ${forward})
		while((${_tmpIDX} GREATER -1) AND (${_count} LESS ${_index}))
			_STRING_SUBSTRING("${_input}" _input _cIDX ${_tmpIDX} ${rLength} ${forward})
			STRING_FIND("${_input}" "${replace}" _tmpIDX ${forward})
			math(EXPR _count "${_count}+1")
		endwhile()	
		
		if(_count EQUAL ${_index})
			if(${forward})
				math(EXPR _count "${_count}-1")
				math(EXPR _cIDX "${_cIDX} + ${_count} * ${rLength}")
				string(SUBSTRING "${input}" 0 ${_cIDX} _s)
				set(${output} "${_s}${text}${_input}")
			else()
				string(LENGTH "${input}" _length)
				math(EXPR _cIDX "${_cIDX}+${rLength}")
				math(EXPR _length "${_length}-${_cIDX}")
				string(SUBSTRING "${input}" ${_cIDX} ${_length} ${output})
				set(${output} "${_input}${text}${${output}}")
			endif()
		else()
			set(${output} "${input}")
		endif()
	else()
		set(${output} "${input}")
	endif()

endmacro(STRING_REPLACE_ONE)

# Makro do podmiany w stringu tylko pierwszego pasującego z lewej lub prawej ciągu znaków

macro(STRING_REPLACE_FIRST input replace text output forward)	

	STRING_REPLACE_ONE("${input}" "${replace}" "${text}" ${output} 0 ${forward})

endmacro(STRING_REPLACE_FIRST)