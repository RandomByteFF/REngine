file(GLOB SPV_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.spv")

add_custom_target(copy_spv_files
  COMMAND ${CMAKE_COMMAND} -E echo "Copying .spv files..."
)

if (WIN32)
add_custom_command(
	TARGET copy_spv_files POST_BUILD
	COMMAND compile.bat
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
else()
add_custom_command(
	TARGET copy_spv_files POST_BUILD
	COMMAND sh ./compile.sh
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
endif()

foreach(SPV_FILE ${SPV_FILES})
	get_filename_component(SPV_FILENAME ${SPV_FILE} NAME)

	set(OUTPUT_SPV_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SPV_FILENAME}")

	add_custom_command(TARGET copy_spv_files POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${SPV_FILE}" "${OUTPUT_SPV_FILE}"
		COMMENT "Copying ${SPV_FILENAME}"
	)
endforeach()