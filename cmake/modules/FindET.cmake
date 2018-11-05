###  The path for ET is restricted to a single copy on the Hall A cluster
###  which has been compiled for 64-bit architecture.
###  In principle this could be generalized, but would need to have guards
###  to verify the library has been compiled with the correct flag.


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

message("Foud ET:  ${ET_INCLUDE_DIR},  ${ET_LIBRARY}")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ET "The ET libraries were not found; online analysis will be disabled." ET_INCLUDE_DIR ET_LIBRARY)
