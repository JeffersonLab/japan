cmake_minimum_required(VERSION 3.5)

# Name of this project
project(QwAnalysis VERSION 0.1 LANGUAGES CXX)

# Local path for cmake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/")

# Default install path is the source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "    Install-prefix was at default -> forcing it to the source-dir" )
    message(STATUS "    Use -DCMAKE_INSTALL_PREFIX=/usr/local to set to something else" )
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}"
         CACHE PATH "default install path" FORCE)
endif()

# Use our modules and utility functions
include(CMakeEnv)

# Install in GNU-style directory layout
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}")

# Mac support
message(STATUS "System name ${CMAKE_SYSTEM_NAME}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif()
endif()


#----------------------------------------------------------------------------
# Sources and headers
#
file(GLOB my_project_sources
  Analysis/src/*.cc
  Parity/src/*.cc
  )
file(GLOB my_project_headers
  Analysis/include/*.h
  Parity/include/*.h
  )

#----------------------------------------------------------------------------
# MYSQLPP
#
find_package(MYSQLPP)
IF(MYSQLPP_FOUND)
  config_add_dependency(MYSQLPP)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  ## KLUDGE:  For now, do not define the __USE_DATABASE__ flag, because we're still missing some of the database support files. --- P.King, 05jun2018
  message(STATUS "  The \"__USE_DATABASE__\" flag is commented out in CMakeLists.txt; database support is DISABLED.")
  #  add_definitions(-D__USE_DATABASE__)
  message(STATUS "  Forcing MYSQLPP_LIBRARIES to \"\".")
  set(MYSQLPP_LIBRARIES "")
ELSE(MYSQLPP_FOUND)
  set(MYSQLPP_LIBRARIES "")
  list(REMOVE_ITEM my_project_sources
    Analysis/src/QwDatabase.cc
  )
endif(MYSQLPP_FOUND)

#----------------------------------------------------------------------------
# Boost
#
find_package(Boost COMPONENTS program_options filesystem system regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

#----------------------------------------------------------------------------
# ROOT
#
set(minimum_root_version 6.0)
find_package(ROOT ${minimum_root_version} REQUIRED)
config_add_dependency(ROOT ${minimum_root_version})


#----------------------------------------------------------------------------
# gitinfo.hh
#
add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh.does_not_exist
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/pullgitinfo.py ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Getting Git information"
  )
add_custom_command(
  OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh
  COMMAND echo -n
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh.does_not_exist
  )
set_property(
  SOURCE Analysis/src/QwOptions.cc Analysis/src/QwRunCondition.cc
  APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh
  )


#----------------------------------------------------------------------------
# Compiler flags
#
set_compiler_flags("${ROOT_CXX_FLAGS}")
#set_diagnostic_flags(WALL WEXTRA)
set_diagnostic_flags(WALL)
report_build_info()


#----------------------------------------------------------------------------
# evio library
#
add_subdirectory(evio)
include_directories(evio/include)

#----------------------------------------------------------------------------
# main dictionaries
#
message(STATUS "Generating ${PROJECT_NAME} dictionaries for ROOT ${ROOT_VERSION}...")
foreach(file ${my_project_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dict)
    set(linkdef ${file_path}/${file_name}LinkDef.h)
    if(NOT EXISTS ${linkdef})
      file(TOUCH ${linkdef})
    endif()
    build_root_dictionary(${file_name} ${file}
      TARGETS ${PROJECT_NAME}
      LINKDEF ${linkdef}
    )
    # Add dictionary to project sources
    set(my_project_sources ${my_project_sources} ${dict}.cxx)
  endif()
endforeach()


#----------------------------------------------------------------------------
# main library
#
add_library(${PROJECT_NAME} SHARED ${my_project_sources} ${my_project_headers}
  ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Parity/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  )

target_compile_options(${PROJECT_NAME}
  PUBLIC
    ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
  PRIVATE
    ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    evio
  PUBLIC
    ROOT::Libraries
    ${MYSQLPP_LIBRARIES}
    ${Boost_LIBRARIES}
  )

install(TARGETS ${PROJECT_NAME}
  EXPORT ${MAIN_PROJECT_NAME_LC}-exports
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(FILES ${my_project_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#----------------------------------------------------------------------------
# executables
#
file(GLOB exefiles
  Parity/main/*.cc
  Analysis/main/*.cc
)
foreach(file ${exefiles})
  get_filename_component(filename ${file} NAME_WE)
  string(TOLOWER ${filename} filelower)

  add_executable(${filelower} ${file})

  target_link_libraries(${filelower}
    PRIVATE
      ${PROJECT_NAME}
  )
  target_compile_options(${filelower}
    PUBLIC
      ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
    PRIVATE
      ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )
  if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
    target_compile_options(${filelower} PUBLIC -fPIC)
  endif()

  install(TARGETS ${filelower}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endforeach()


#----------------------------------------------------------------------------
# uninstall
#
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


#----------------------------------------------------------------------------
# cmake config
#
add_subdirectory(cmake)

