cmake_minimum_required(VERSION 3.5)


#----------------------------------------------------------------------------
# Sources and headers
#
file(GLOB my_evio_headers
  include/THaCoda*.h
  include/evio.h
  )
file(GLOB my_evio_sources
  src/THaCoda*.C
  src/evio.C
  src/swap_util.C
  )


#----------------------------------------------------------------------------
# CODA ET
#
find_package(ET)
IF(ET_FOUND)
  add_definitions(-D__CODA_ET)
  include_directories(${ET_INCLUDE_DIR})
  list(APPEND my_evio_headers
    include/THaEtClient.h
    )
  list(APPEND my_evio_sources
    src/THaEtClient.C
    )
else(ET_FOUND)
  set(ET_LIBRARY "")
endif(ET_FOUND)


#----------------------------------------------------------------------------
# dictionaries
#
message(STATUS "Generating evio dictionaries for ROOT ${ROOT_VERSION}...")
foreach(file ${my_evio_headers})
  # Find the header files that have uncommented ClassDef lines
  execute_process(COMMAND grep -l "[^/]ClassDef" ${file} OUTPUT_VARIABLE result)
  if(result)
    # Get path and filename without extension
    get_filename_component(file_path ${file} PATH)
    get_filename_component(file_name ${file} NAME_WE)
    # Add dictionary target with possible LinkDef file
    set(dict ${CMAKE_CURRENT_BINARY_DIR}/${file_name}Dict)
    set(linkdef ${file_path}/${file_name}LinkDef.h)
    build_root_dictionary(${file_name} ${file}
      TARGETS evio
      LINKDEF ${linkdef}
    )
    # Add dictionary to evio sources
    set(my_evio_sources ${my_evio_sources} ${file_name}Dict.cxx)
  endif()
endforeach()


#----------------------------------------------------------------------------
# evio library
#
add_library(evio SHARED ${my_evio_sources})

target_include_directories(evio
  PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  )

target_compile_options(evio
  PUBLIC
    ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
  PRIVATE
    ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )

target_link_libraries(evio
  PUBLIC
    ROOT::Libraries
  )

install(TARGETS evio
  EXPORT ${MAIN_PROJECT_NAME_LC}-exports
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

install(FILES ${my_evio_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

