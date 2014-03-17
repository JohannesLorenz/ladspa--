#
# General
#

cmake_minimum_required(VERSION 2.6)
project(ladspa++)

SET(MYPROJECT "ladspa++")
SET(MYDECSRIPTION "C++ wrapper for ladspa")
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

# help
# MESSAGE("This is the Makefile for ${MYPROJECT} (version ${VERSION}).")
# MESSAGE("Options: (-D...=..., cmake -L for details)")

#
# Compile
#

# FLAGS
add_definitions(-std=c++11 -Wall -Werror -O3 -pipe)

add_subdirectory(doc)
add_subdirectory(examples)

#
# CPack Stuff
#

INCLUDE(MyCPack)
