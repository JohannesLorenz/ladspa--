# #-- Add an Option to toggle the generation of the API documentation
# option(BUILD_DOCUMENTATION "Use Doxygen to create the API documentation" ON)
# if(BUILD_DOCUMENTATION)
# 	FIND_PACKAGE(Doxygen)
# 	if (NOT DOXYGEN_FOUND)
# 		message(FATAL_ERROR 
# 		"Doxygen is needed to build the documentation.")
# 	endif()
# 	#-- Configure the Template Doxyfile for our specific project
# 	configure_file(Doxyfile.in 
# 		${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
# 	#-- Add a custom target to run Doxygen when ever the project is built
# 	add_custom_target (Docs
# 		COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
# 		SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
# 	# IF you do NOT want the documentation to be generated EVERY time you build the project
# 	# then leave out the 'ALL' keyword from the above command.
# endif()





find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)