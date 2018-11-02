# Require at least cmake 2.8.4
cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

# Name of this project
project(QwAnalysis)

# Local path for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# C++11 standard if avialable
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++11 support.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++0x support.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

# Remove some compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning -Wno-deprecated-declarations -Wno-nonnull-compare")

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
  )
file(GLOB my_project_sources
#  ###  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/*.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/*.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Tracking/src/*.cc
)
list(APPEND my_project_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwCodaControlEvent.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwF1TDC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwMockable.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/MQwV775TDC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwColor.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwControl.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwDatabase.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwDBInterface.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwEPICSEvent.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwEventBuffer.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwF1TDContainer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwHistogramHelper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwLog.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwOmnivore.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwOptions.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwParameterFile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwPMT_Channel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwPromptSummary.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRint.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRootFile.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwRunCondition.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwScaler_Channel.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwScanner.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_Accumulator.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_Channel.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_LogicalAccumulator.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSIS3320_Samples.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwSubsystemArray.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwTypes.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwVQWK_Channel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/VQwHardwareChannel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/VQwSubsystem.cc
)
list(APPEND my_project_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/VQwDataHandler.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBCM.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBeamDetectorID.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBeamLine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBeamMod.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBlinder.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBPMCavity.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBPMStripline.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwClock.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCombinedBCM.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCombinedBPM.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCombinedPMT.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwComptonElectronDetector.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwComptonPhotonDetector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwDetectorArray.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwEnergyCalculator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwEventRing.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwFakeHelicity.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwHaloMonitor.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwHelicity.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwHelicityPattern.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwIntegratedRaster.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwIntegratedRasterChannel.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwIntegrationPMT.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwLinearDiodeArray.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwLumi.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwMainCerenkovDetector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwMollerDetector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwBlindDetectorArray.cc
#  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwParityDB.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwQPD.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCombiner.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCombinerSubsystem.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwScaler.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwSubsystemArrayParity.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/VQwBCM.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/VQwBPM.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/VQwClock.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwkRegBlueCorrelator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/LinReg_Bevington_Pebay.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/QwCorrelator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Parity/src/LRBCorrector.cc
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
set(ROOT_LIBRARIES "${ROOT_LIBRARIES} -lMinuit2")
execute_process(COMMAND "${ROOTSYS}/root-config --cflags" OUTPUT_VARIABLE ROOT_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

# Load MYSQLPP
find_package(MYSQLPP)
IF(MYSQLPP_FOUND)
  include_directories(${MYSQLPP_INCLUDE_DIR})
  ## KLUDGE:  For now, do not define the __USE_DATABASE__ flag, because we're still missing some of the database support files. --- P.King, 05jun2018
  message("--   The \"__USE_DATABASE__\" flag is commented out in CMakeLists.txt; database support is DISABLED.")
  #  add_definitions(-D__USE_DATABASE__)
  list(APPEND my_project_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwDatabase.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/src/QwDBInterface.cc
    )
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

# Create dictionaries for headers that contain "ClassDef"
if(ROOT_VERSION VERSION_LESS 5.90)
message("Generating QwAnalysis dictionaries for ROOT ${ROOT_VERSION}...")
foreach(file ${my_evio_headers} ${my_project_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dictionary)
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/dictionary/${file_name}Dict)
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
else()
message("No QwAnalysis dictionaries needed for ROOT ${ROOT_VERSION}.")
endif()


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

# Qweak configure script
set(QWANALYSIS ${CMAKE_SOURCE_DIR})
set(QW_BIN ${CMAKE_SOURCE_DIR}/bin)
set(QW_LIB ${CMAKE_SOURCE_DIR}/lib)
configure_file(${CMAKE_SOURCE_DIR}/qweak-config.in qweak-config @ONLY)

# Link the evio library
add_library(evio SHARED ${my_evio_sources})
target_link_libraries(evio)
install(TARGETS evio DESTINATION ${CMAKE_SOURCE_DIR}/lib)

# Link the ROOT libraries
add_library(QwAnalysis SHARED ${CMAKE_CURRENT_BINARY_DIR}/include/gitinfo.hh ${my_project_sources} ${my_project_headers})
target_link_libraries(QwAnalysis
  evio ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES} ${Boost_LIBRARIES})
install(TARGETS QwAnalysis DESTINATION ${CMAKE_SOURCE_DIR}/lib)

# Install pcm files into lib
install(FILES ${pcmfiles} DESTINATION ${CMAKE_SOURCE_DIR}/lib)

# Add the target executables
file(GLOB exefiles
  ${CMAKE_SOURCE_DIR}/Parity/main/*.cc
  ${CMAKE_SOURCE_DIR}/Analysis/main/*.cc
#  ${CMAKE_SOURCE_DIR}/Tracking/main/*.cc
  )
foreach(file ${exefiles})
  get_filename_component(filename ${file} NAME_WE)
  string(TOLOWER ${filename} filelower)
  add_executable(${filelower} ${file})
  target_link_libraries(
    ${filelower}
    ${ROOT_LIBRARIES} ${MYSQLPP_LIBRARIES} ${Boost_LIBRARIES} QwAnalysis evio
    )
  install(TARGETS ${filelower} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
endforeach()

