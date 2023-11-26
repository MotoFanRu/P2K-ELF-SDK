add_custom_command(
	TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_POSTLINK} ${PROJECT_NAME}_raw.elf -o ${PROJECT_NAME}.elf
	COMMENT "Postlinking ${PROJECT_NAME}_raw.elf to ${PROJECT_NAME}.elf"
	VERBATIM
)
