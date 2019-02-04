###  The path for EPICS is restricted to a single copy on the Hall A cluster
###  which has been compiled for 64-bit architecture.
###  In principle this could be generalized, but would need to have guards
###  to verify the library has been compiled with the correct flag.


find_library(EPICS_CA_LIBRARY ca
  PATHS /adaqfs/apps/epics/lib/linux-x86_64
  NO_DEFAULT_PATH
  DOC "EPICS CA library"
  )
find_library(EPICS_CAS_LIBRARY cas
  PATHS /adaqfs/apps/epics/lib/linux-x86_64
  NO_DEFAULT_PATH
  DOC "EPICS CAS library"
  )
find_library(EPICS_COM_LIBRARY Com
  PATHS /adaqfs/apps/epics/lib/linux-x86_64
  NO_DEFAULT_PATH
  DOC "EPICS COM library"
  )
find_library(EPICS_GDD_LIBRARY gdd
  PATHS /adaqfs/apps/epics/lib/linux-x86_64
  NO_DEFAULT_PATH
  DOC "EPICS GDD library"
  )
#  Needs to build this line: -L/adaqfs/apps/epics/lib/linux-x86_64 -lca -lcas -lCom -lgdd 

find_path(EPICS_INCLUDE_DIR
  NAMES cadef.h
  PATHS /adaqfs/apps/epics/include
  #          $ENV{CODA}/common/include
  DOC "EPICS header include base directory"
  )
#  Needs to get all three: -I /adaqfs/apps/epics/include -I /adaqfs/apps/epics/include/os/Linux -I /adaqfs/apps/epics/include/compiler/gcc

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EPICS "The EPICS libraries were not found; feedback analysis will be disabled."  EPICS_CA_LIBRARY EPICS_CAS_LIBRARY EPICS_COM_LIBRARY EPICS_GDD_LIBRARY EPICS_INCLUDE_DIR)
