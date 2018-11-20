# Require at least cmake 2.8.4
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

# Name of this project
project(QwAnalysis)

# Default install path is the source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "    Install-prefix was at default -> forcing it to the source-dir" )
    message(STATUS "    Use -DCMAKE_INSTALL_PREFIX=/usr/local to set to something else" )
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
endif()

message(STATUS "System name ${CMAKE_SYSTEM_NAME}")
#MAC specific variable
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

# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# Add the project include directory
include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include
  )

# Put all QwAnalysis files in my_project_sources
file(GLOB my_project_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/include/*.h
  ${CMAKE_CURRENT_BINARY_DIR}/include/QwSVNVersion.h
  )
file(GLOB my_project_sources
  ###  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/*.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/*.cc
  )
list(APPEND my_project_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwCodaControlEvent.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwF1TDC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwMockable.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwV775TDC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwColor.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwEPICSEvent.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwEventBuffer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwF1TDContainer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwHistogramHelper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwLog.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwOmnivore.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwOptions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwParameterFile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwPMT_Channel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwPromptSummary.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRint.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRootFile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRunCondition.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwScaler_Channel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_Accumulator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_LogicalAccumulator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_Samples.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSubsystemArray.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwTypes.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwVQWK_Channel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/VQwHardwareChannel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/VQwSubsystem.cc
)

# Put all evio files in my_project_sources (exclude THaEtClient)
file(GLOB my_evio_headers
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/THaCoda*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/evio.h
  )
file(GLOB my_evio_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/THaCoda*.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/evio.C
  ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/swap_util.C
  )

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})
set(ROOT_LIBRARIES ${ROOT_LIBRARIES} -lNew -lGui -lMinuit2)
list(APPEND CMAKE_INSTALL_RPATH ${ROOT_LIBRARY_DIR})

# Load MYSQLPP
find_package(MYSQLPP)
IF(MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  ## KLUDGE:  For now, do not define the __USE_DATABASE__ flag, because we're still missing some of the database support files. --- P.King, 05jun2018
  message("--   The \"__USE_DATABASE__\" flag is commented out in CMakeLists.txt; database support is DISABLED.")
  #  add_definitions(-D__USE_DATABASE__)
  #  list(APPEND my_project_sources
  #    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwDatabase.cc
  #    )
  message("--   Forcing MYSQLPP_LIBRARIES to \"\".")
  SET(MYSQLPP_LIBRARIES "")
  list(APPEND CMAKE_INSTALL_RPATH ${MYSQLPP_LIBRARY_DIR})
ELSE(MYSQLPP_FOUND)
  SET(MYSQLPP_LIBRARIES "")
endif(MYSQLPP_FOUND)

# Find Boost libraries
# There seems to be a bug starting in version 2.8.4 of CMake's boost
# finding process. As such, we'll disable it here and require 2.8.4 or higher
SET( Boost_NO_BOOST_CMAKE ON )
find_package(Boost COMPONENTS program_options filesystem system regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})
list(APPEND CMAKE_INSTALL_RPATH ${Boost_LIBRARY_DIRS})

# Find the CODA ET libaries
find_package(ET)
IF(ET_FOUND)
  add_definitions(-D__CODA_ET)
  include_directories(${ET_INCLUDE_DIR})
  list(APPEND my_evio_headers
    ${CMAKE_CURRENT_SOURCE_DIR}/evio/include/THaEtClient.h
    )
  list(APPEND my_evio_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/evio/src/THaEtClient.C
    )
else(ET_FOUND)
  SET(ET_LIBRARY "")
endif(ET_FOUND)

add_definitions(-std=c++11)

# Create dictionaries for headers that contain "ClassDef"
message("Generating evio dictionaries for ROOT ${ROOT_VERSION}...")
foreach(file ${my_evio_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dict)
    ROOT_GENERATE_DICTIONARY(
      ${dict}
      ${file}
      OPTIONS -p)
    # Add dictionary to evio sources
    set(my_evio_sources ${my_evio_sources} ${dict}.cxx)
    # Add pcm file to list
    if(ROOT_VERSION VERSION_GREATER 5.90)
    set(pcmfiles ${pcmfiles} ${dict}_rdict.pcm)
    endif()
  endif()
endforeach()
message("Generating QwAnalysis dictionaries for ROOT ${ROOT_VERSION}...")
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
    if(EXISTS ${linkdef})
      ROOT_GENERATE_DICTIONARY(
        ${dict}
        ${file}
        LINKDEF ${linkdef}
        OPTIONS -p)
    else()
      ROOT_GENERATE_DICTIONARY(
        ${dict}
        ${file}
        OPTIONS -p)
    endif()
    # Add dictionary to project sources
    set(my_project_sources ${my_project_sources} ${dict}.cxx)
    # Add pcm file to list
    if(ROOT_VERSION VERSION_GREATER 5.90)
    set(pcmfiles ${pcmfiles} ${dict}_rdict.pcm)
    endif()
  endif()
endforeach()

# Add target of QwGitVersion.h
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
  SOURCE ${CMAKE_SOURCE_DIR}/Analysis/src/QwOptions.cc ${CMAKE_SOURCE_DIR}/Analysis/src/QwRunCondition.cc
  APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh
  )

# Set rpath
list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Link the evio library
add_library(evio SHARED ${my_evio_sources})
target_link_libraries(evio)
install(TARGETS evio DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

# Link the ROOT libraries
add_library(QwAnalysis SHARED ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh ${my_project_sources} ${my_project_headers})
target_link_libraries(QwAnalysis
  evio ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS QwAnalysis DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")

# Install pcm files into lib
install(FILES ${pcmfiles} DESTINATION ${CMAKE_SOURCE_DIR}/lib)

# Add the target executables
file(GLOB exefiles
  ${CMAKE_SOURCE_DIR}/Parity/main/*.cc
  ${CMAKE_SOURCE_DIR}/Analysis/main/*.cc
  )
foreach(file ${exefiles})
  get_filename_component(filename ${file} NAME_WE)
  string(TOLOWER ${filename} filelower)
  add_executable(${filelower} ${file})
  target_link_libraries(
    ${filelower}
    ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES} ${Boost_LIBRARIES} QwAnalysis evio ${ET_LIBRARY}
    )
  install(TARGETS ${filelower} DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
endforeach()

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
