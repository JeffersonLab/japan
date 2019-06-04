#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(camguin)

# Default install path is the source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "    Install-prefix was at default -> forcing it to the source-dir" )
    message(STATUS "    Use -DCMAKE_INSTALL_PREFIX=/usr/local to set to something else" )
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}"
        CACHE PATH "default install path" FORCE )
endif()

# Use GNU install dirs
include(GNUInstallDirs)

list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
list(APPEND CMAKE_INSTALL_RPATH "${ROOT_LIBRARY_DIR}")


#----------------------------------------------------------------------------
# generate dictionaries
#
ROOT_GENERATE_DICTIONARY(
    camguinDict                         # path to dictionary to generate
    "camguin.hh"           # list of classes to process
    #LINKDEF "include/camguinLinkDef.h"  # ROOT linkDef file
    OPTIONS -p
)

#----------------------------------------------------------------------------
#
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/regressor)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/camguin*.C)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/cam*.hh)

#----------------------------------------------------------------------------
# Generate dictionaries as well (so we can write objects to ROOT files
#
add_library(camguin-lib SHARED ${sources} ${headers} camguinDict.cxx)
set_target_properties(camguin-lib PROPERTIES OUTPUT_NAME camguin)
target_link_libraries(camguin-lib ${ROOT_LIBRARIES})

add_executable(camguin-bin camguin_compile.C)
set_target_properties(camguin-bin PROPERTIES OUTPUT_NAME camguin)
target_link_libraries(camguin-bin camguin-lib)

#----------------------------------------------------------------------------
#
add_custom_target(camguin DEPENDS camguin-bin)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS camguin-bin DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS camguin-lib DESTINATION ${CMAKE_INSTALL_LIBDIR})
