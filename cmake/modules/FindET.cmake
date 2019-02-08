###  The path for ET is restricted to a single copy on the Hall A cluster
###  which has been compiled for 64-bit architecture.
###  In principle this could be generalized, but would need to have guards
###  to verify the library has been compiled with the correct flag.

set(arch ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})

# if( DEFINED ENV{CODA} )
  find_library(ET_LIBRARY et
    PATHS /adaqfs/home/apar/et-12.0/src/libsrc/.Linux-x86_64
    #         $ENV{CODA}/${arch}/lib
    #         $ENV{CODA}/${CMAKE_SYSTEM_NAME}/lib
    NO_DEFAULT_PATH
    DOC "Event Transport (ET) library"
    )
  find_path(ET_INCLUDE_DIR
    NAMES et.h
    PATHS /adaqfs/home/apar/et-12.0/src/libsrc
    #          $ENV{CODA}/common/include
    NO_DEFAULT_PATH
    DOC "Event Transport (ET) header include directory"
    )
# endif()

if(NOT TARGET EVIO::ET AND ET_LIBRARY AND ET_INCLUDE_DIR)
  add_library(EVIO::ET SHARED IMPORTED)
  set_target_properties(EVIO::ET PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ET_INCLUDE_DIR}"
    IMPORTED_LOCATION "${ET_LIBRARY}"
    )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ET ET_INCLUDE_DIR ET_LIBRARY)
