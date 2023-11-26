function(redefine_sym SYM_IN SYM_OUT PATCH_FILE EXT)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		PRE_LINK
		COMMAND ${CMAKE_OBJCOPY} --redefine-sym ${SYM_IN}=${SYM_OUT} "CMakeFiles/${PROJECT_NAME}.dir/${PATCH_FILE}${EXT}"
		COMMENT "Redefining ${SYM_IN} to ${SYM_OUT} in ${PATCH_FILE}${EXT}"
		VERBATIM
	)
endfunction()
