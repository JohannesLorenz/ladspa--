#
# General
#

cmake_minimum_required(VERSION 2.6)
project(ladspa++)

#
# Compile
#

# FLAGS
add_definitions(-std=c++11 -Wall -Werror -O3 -pipe)

add_subdirectory(doc)
add_subdirectory(examples)

#
# Installation
#

install(FILES src/ladspa++.h DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION "${CMAKE_INSTALL_PREFIX}/share/ladspa++/doc/")
install(FILES README.txt LICENSE.txt DESTINATION "${CMAKE_INSTALL_PREFIX}/share/ladspa++/doc/")
# TODO: install docs
