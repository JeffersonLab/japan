cmake_minimum_required(VERSION 3.5)

#----------------------------------------------------------------------------
# ROOT
#
set(minimum_root_version 6.0)
find_package(ROOT ${minimum_root_version} REQUIRED)
config_add_dependency(ROOT ${minimum_root_version})

#----------------------------------------------------------------------------
# CODA ET
#
find_package(ET)

#----------------------------------------------------------------------------
# EPICS
find_package(EPICS)

message(STATUS "***********  In Feedback Cmake")
set_compiler_flags("${ROOT_CXX_FLAGS}")
#set_diagnostic_flags(WALL WEXTRA)
set_diagnostic_flags(WALL)
report_build_info()

#----------------------------------------------------------------------------
#  Feedback needs both ET and EPICS
IF(ET_FOUND)
IF(EPICS_FOUND)
  include_directories(${ROOT_INCLUDE_DIR})
  include_directories(${ET_INCLUDE_DIR})
  include_directories(${EPICS_INCLUDE_DIR}
                      ${EPICS_INCLUDE_DIR}/os/Linux
		      ${EPICS_INCLUDE_DIR}/compiler/gcc)
  include_directories(${PROJECT_SOURCE_DIR}/Analysis/include
                      ${PROJECT_SOURCE_DIR}/Parity/include)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

 set(feedback_sources
   ${CMAKE_CURRENT_SOURCE_DIR}/src/GreenMonster.cc
   ${CMAKE_CURRENT_SOURCE_DIR}/src/QwEPICSControl.cc
   ${CMAKE_CURRENT_SOURCE_DIR}/src/QwHelicityCorrelatedFeedback.cc
   ${CMAKE_CURRENT_SOURCE_DIR}/src/cfSockCli.cc
 )
 add_library(QwFeedback SHARED ${feedback_sources})
 target_include_directories(QwFeedback  PUBLIC
			    ${CMAKE_CURRENT_SOURCE_DIR}/include
			    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
			    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
			    )
 target_compile_options(QwFeedback
   PUBLIC
    ${${PROJECT_NAME_UC}_CXX_FLAGS_LIST}
  PRIVATE
    ${${PROJECT_NAME_UC}_DIAG_FLAGS_LIST}
  )
target_link_libraries(QwFeedback
  PUBLIC
    EVIO::ET
    ROOT::Libraries
    ${EPICS_CA_LIBRARY} ${EPICS_CAS_LIBRARY} ${EPICS_COM_LIBRARY} ${EPICS_GDD_LIBRARY}
  )

install(TARGETS QwFeedback
  EXPORT ${MAIN_PROJECT_NAME_LC}-exports
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

install(FILES ${my_feedback_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

endif(EPICS_FOUND)
endif(ET_FOUND)



