# Makefile for Qweak analysis code : make required
# P. M. King
# 2006-11-07
#
# Modifications:
#----------------------------------------------------------------------
#
#
#----------------------------------------------------------------------

############################
############################
# User's compilation and linkage flags, plus other customization variables :
############################
############################

DEBUG := -g -O0
# Add -g if you need to debug (but you'd better
# first type 'make distclean' to enable full
# recompilation with this flag).
# Add -O0 if you find that the compiler optimizes
# away some of the variables you are interested in.

OPTIM  := -O
# Default, safe, optimization flag understood by most compliers. Set
# optmization flag(s) in the architecture dependent sections instead.

DEFAULTADD = $(ADD)
# Should contain extra flag to pass to the precompiler
# ADD is a variable you should not set in this Makefile but either
# in your.cshrc file or from the prompt line when you type
# "make [config] 'ADD=-D__PADDLES'" instead of simply "make [config]"
# Use DEFAULTADD to set extra flags you always want to have for your
# customized compiling e.g replace above line "DEFAULTADD := $(ADD)" by
# "DEFAULTADD := $(ADD) -D__PADDLES"


############################
############################
# Extensions for files :
# Ought not to be modified, thus your nomenclature should abide by
# these standards unless everybody agrees on changing them.
# (Which sure would be painful).
############################
############################

ObjSuf  := .o
SrcSuf  := .cc
ExeSuf  :=
DllSuf  := .so
IncSuf  := .h



############################
############################
# Shell commands called :
# You might have to modify default flags and/or choice of command to port
# the Makefile.
############################
############################

AWK      := awk
BASENAME := basename
CAT      := cat
CD       := cd
CHMOD    := chmod
DIRNAME  := dirname
ECHO     := echo
FIND     := find
GCC      := g++
      # This must be the GNU compiler collection : explicit use of
      # flag '-M' for automatic search for dependencies
      # It is not correlated to $(CXX) and $(LD) which depend on $(ARCH)
GREP     := grep
LS       := ls
MAKE     := make
RM       := \rm -f
      # 'rm' is often aliases as 'rm -i', so '\rm' instead
CP       := \cp
ROOTCINT := rootcint
SED      := sed -e
TOUCH    := touch



############################
############################
# Environment :
############################
############################

ifeq ($(QwSHELL),)
SHELL := bash
else
SHELL := $(QwSHELL)
endif
      # Warning : Explicit use of statements "for in; do ;done;"
      # and "if []; then ; fi;
      # Should be bash shell or alike (ksh on ifarms1)
ARCH  := $(shell uname)
      # Operating system


############################
############################
# Modularity :
#
#   The "EXCLUDEDIRS" list should contain directories which are not part
#   of the QwAnalysis standard package, and which should not be built
#   by this Makefile.  If they should be built automatically, a call
#   to their own Makefile will be made from a specfic target.
#   See the "myevio_lib" target, as an example.
#
############################
############################

EXCLUDEDIRS = evio Extensions

ifeq ($(strip $(shell $(ECHO) $$(if [ -e .EXES ]; then $(CAT) .EXES; fi))),)
  ifneq ($(CODA),)
    #  The realtime executables should be added in this section.
    EXES := qwtracking qwparity qwsimtracking qwcompton qwmoller qwroot
  else
    EXES := qwtracking qwparity qwsimtracking qwcompton qwmoller qwroot
  endif
else
  EXES := $(shell $(ECHO) $$(if [ -e .EXES ]; then $(CAT) .EXES; fi))
endif
ifeq ($(filter config,$(MAKECMDGOALS)),config)
  ifneq ($(CODA),)
    #  The realtime executables should be added in this section.
    EXES := qwtracking qwparity qwsimtracking qwcompton qwmoller qwroot
  else
    EXES := qwtracking qwparity qwsimtracking qwcompton qwmoller qwroot
  endif
endif
EXES := qwparity_simple qwmockdatagenerator
# overridden by "make 'EXES=exe1 exe2 ...'"

ifneq ($(filter qwrealtime,$(EXES)),)
 ifeq ($(CODA),)
  $(error qwrealtime requires CODA)
  # With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
 endif
endif

#
#  Get information about the SVN environment of this directory, including the 
#  revision number, and the repository URL.
#
QWANA_VERSION_H        := Analysis/include/QwSVNVersion.h
QWANA_SVN_REVISION     := $(strip $(shell svnversion 2>/dev/null || echo "unknown_revision"))
QWANA_SVN_REVISION_OLD := $(strip $(shell test -e $(QWANA_VERSION_H) && \
				($(GREP) "QWANA_SVN_REVISION" $(QWANA_VERSION_H)|$(AWK) -F'"' '{print $$2}')))
QWANA_SVN_URL          := $(strip $(shell (svn info 2>/dev/null || echo "URL: unknown_URL") \
				| $(AWK) '$$1=="URL:"{print $$2}'))
QWANA_SVN_LASTREVISION := $(strip $(shell (svn info 2>/dev/null || echo "Last Changed Rev: unknown_revision") \
				| $(AWK) -F":" '$$1=="Last Changed Rev"{print $$2}'))


############################
############################
# Cern ROOT package related variables :
############################
############################


ifndef ROOTSYS
  ROOTSYS := $(shell root-config --prefix)
  ifndef ROOTSYS
    $(warning ROOTSYS variable is not defined and root-config did not return a location.)
    $(warning Use the script SetupFiles/SET_ME_UP.csh or SetupFiles/SET_ME_UP.bash first.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: No ROOT installation could be found)
  endif
endif
ROOTCONFIG   := $(ROOTSYS)/bin/root-config
## ROOTDEFINE   := $(shell $(ROOTCONFIG) --features | $(SED) 's/\(\s*\)\([a-zA-Z0-9_]*\)/\1-D__ROOT_HAS_\2/g;y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/')
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --new --libs) -lTreePlayer -lGui -lMinuit2
        # -lNew : for map file capability
        # -lTreePlayer -lProof : for user loops calling tree
        #                        variables under conditions
#ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# -lMathMore : for using ROOT advanced math library
ifeq ($(shell $(ROOTCONFIG) --has-mathmore),yes)
  ROOTCFLAGS += -D__ROOT_HAS_MATHMORE
  ROOTLIBS += -lMathMore
else
  $(warning The ROOT MathMore plugin was not found on your system.  Expect reduced functionality.)
endif

############################
############################
# Qw Paths :
# They are set when $(QWANALYSIS)/SetupFiles/.Qwcshrc (or .bash)
# is sourced prior to the call for this Makefile.
# A priori they won't be modified. They are (don't uncomment) :
# QWANALYSIS := /home/lenoble/QwAnalysis
# Not the actual value, but $(MAKE) is run from
# this directory
# QW_BIN      := $(QWANALYSIS)/bin
# QW_LIB      := $(QWANALYSIS)/lib
############################
############################
#  These next lines check the paths and exit if there is a problem.
############################
############################

ifndef QWANALYSIS
  $(warning Warning : QWANALYSIS variable is not defined.  Setting to current directory.)
  $(error You have probably forgotten to "source SetupFiles/SET_ME_UP.csh")
  QWANALYSIS := $(shell pwd)
endif
ifeq ($(strip $(QWANALYSIS)),)
  $(error Aborting : QWANALYSIS variable is not set.  Source the SetupFiles/.Qwcshrc script first)
endif
ifneq ($(shell test $(QWANALYSIS) -ef $(shell pwd) || echo false),)
  $(error Aborting : QWANALYSIS variable disagrees with the working directory.  Source the SetupFiles/.Qwcshrc script first)
endif

#  If an OS and hardware specific subdirectory is present,
#  we will use the "bin" and "lib" directories in it,
#  instead of the base "bin" and "lib" directories.
OS_HW_NAME  := $(uname -s -m | sed 's/ /_/g')
INSTALL_DIR := $(strip $(shell $(ECHO) $(QWANALYSIS)$$( (if [ -d $(OS_HW_NAME)]; then $(ECHO) "/"$(OS_HW_NAME); fi))))

ifndef QW_BIN
  $(warning Warning : QW_BIN variable is not defined.  Setting to QWANALSYIS/bin.)
  QW_BIN := $(QWANALYSIS)/bin
endif
ifneq ($(shell test $(QW_BIN) -ef $(INSTALL_DIR)/bin || echo false),)
  $(error Aborting : QW_BIN variable is not set properly  Source the SetupFiles/.Qwcshrc script first)
endif

ifndef QW_LIB
  $(warning Warning : QW_LIB variable is not defined.  Setting to QWANALSYIS/lib.)
  QW_LIB := $(QWANALYSIS)/lib
endif
ifneq ($(shell test $(QW_LIB) -ef $(INSTALL_DIR)/lib || echo false),)
  $(error Aborting : QW_LIB variable is not set properly  Source the SetupFiles/.Qwcshrc script first)
endif


EVIO := $(QWANALYSIS)/evio



############################
############################
# JLab CODA package related variables :
############################
############################

ifdef CODA
CODACFLAGS   := -I$(CODA)/common/include -D__CODA_ET
CODALIBS     := -L$(CODA_LIB) -let
endif
CODACFLAGS   += -I$(EVIO)/include
CODALIBS     += -L$(QW_LIB) -lmyevio
      # -lmyevio : now integrated in our distribution (April 19 2001) ;
      # Regenerated if necessary ; I had to rewrite CODA
      # group's Makefile in $(EVIO)




############################
############################
# Some set-up for the Boost library use
############################
############################
ifndef BOOST_INC_DIR
  ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name boost)),/usr/include/boost)
    $(warning Install the Boost library on your system, or set the environment)
    $(warning variables BOOST_INC_DIR and BOOST_LIB_DIR to the directory with)
    $(warning the Boost headers and libraries, respectively. The headers should)
    $(warning be in BOOST_INC_DIR/boost, and the library files in BOOST_LIB_DIR.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: Could not find the Boost library)
  endif
  BOOST_INC_DIR = /usr/include
  BOOST_LIB_DIR = /usr/lib
  BOOST_INC  =
  BOOST_LIBS =
else
  ifneq ($(strip $(shell $(FIND) $(BOOST_INC_DIR)/boost -maxdepth 1 -name version.hpp)),$(BOOST_INC_DIR)/boost/version.hpp)
    $(warning Set the environment variables BOOST_INC_DIR and BOOST_LIB_DIR to)
    $(warning the directory with the Boost headers and libraries, respectively.)
    $(warning The Boost header files should be in BOOST_INC_DIR/boost, and the)
    $(warning library files in BOOST_LIB_DIR.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(error   Error: Could not find the Boost library)
  else
    BOOST_INC  = -I${BOOST_INC_DIR}
    BOOST_LIBS = -L${BOOST_LIB_DIR}
  endif
endif

#  Check to see if BOOST_INC_DIR is equal to /usr/include;
#  if so, clear the BOOST_INC flag, but leave BOOST_LIBS unchanged
ifeq ($(shell test $(BOOST_INC_DIR) -ef /usr/include || echo false),)
  BOOST_INC  =
endif

#  We should also put a test on the boost version number here.
BOOST_VERSION = $(shell perl -ane "print /\#define\s+BOOST_VERSION\s+(\S+)/" ${BOOST_INC_DIR}/boost/version.hpp)
ifeq ($(BOOST_VERSION),)
  $(error   Error: Could not determine Boost version)
endif
ifneq ($(shell test $(BOOST_VERSION) -lt "103300" && echo "boost_not_supported"),)
  $(warning Warning: Boost libraries probably too old)
  $(warning Boost library support only for versions 1.33.0 and up)
endif

#  List the Boost libraries to be linked to the analyzer.
ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_filesystem-mt.so)),$(BOOST_LIB_DIR)/libboost_filesystem-mt.so)
  # If multi-threaded libraries exist, use them instead of single threaded libraries
  BOOST_LIBS += -lboost_filesystem-mt -lboost_program_options-mt -lboost_regex-mt
else
  # Otherwise use the single threaded libraries
  BOOST_LIBS += -lboost_filesystem -lboost_program_options -lboost_regex
endif

#  Before boost::filesystem 1.35.0 the system functionality is inside filesystem
ifeq ($(shell test $(BOOST_VERSION) -lt "103500" && echo "boost_system_in_filesystem"),)
  ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_system-mt.so)),$(BOOST_LIB_DIR)/libboost_system-mt.so)
    BOOST_LIBS += -lboost_system-mt
  else
    BOOST_LIBS += -lboost_system
  endif
endif

BOOST_LIBS += -ldl


############################
############################
# Platform dependent variables :
############################
############################
LIBTOOL = $(LD)

ifeq ($(ARCH),Linux)

CXX            := $(GCC)
CXXFLAGS       := -Wall -fPIC -Wno-unknown-warning -Wno-misleading-indentation -Wno-deprecated-declarations -Wno-nonnull-compare
OPTIM          := $(OPTIM)
LD             = $(GCC)
LDFLAGS	       = -Wl,-rpath,$(QW_LIB) -Wl,--no-as-needed
        # -Wl,--no-as-needed: incorrect ordering in root-config --libs
LDLIBS         =
SOFLAGS        = -shared

ROOTCFLAGS   := $(ROOTCFLAGS) -D_REENTRANT
        # -D_REENTRANT : '$(ROOTCONFIG) --cflags' gives incomplete result
        #                on some environment

ROOTLIBS     := $(ROOTLIBS) -lpthread  -lThread
        # -lpthread:  because '$(ROOTCONFIG) --libs' gives incomplete result
        #             on gzero and libet.so requires it
        # -lThread:   Required for compilation on Linux systems with
        #             ROOT 4.04/02 or 5.08/00 (first noted by J-S Real
        #             on 3FEB2006)
endif

ifeq ($(ARCH),SunOS)
CXX            = CC
CXXFLAGS       = -KPIC
OPTIM         := $(subst "-","-x",$(OPTIM))
LD             = CC
LDFLAGS        =
LDLIBS         = -lm -lposix4  -lsocket -lnsl -lresolv -ldl
# These flags were suggested by Carl Timmer on 30May2001 to link properly the
# code on jlabs1
# 2002Feb07, PMK; transfered the flags from "LDFLAGS" to "LDLIBS".
SOFLAGS        = -G
endif

ifeq ($(ARCH),Darwin)

## MacOS 10.5 shipped with g++-4.0 (default) and g++-4.2.
## The older g++ doesn't properly support "covariant returns," so
## specify the newer one in this section.
## Override on the command line with "make GCC=g++-whatever"

GCC            := g++-4.2
ifeq ($(shell which $(GCC)),)
  ## At least one installation of 10.7 comes only with "g++", which
  ## happens to be version 4.2 
  GCC          := g++
endif
CXX            := $(GCC)
CXXFLAGS       := -Wall -fPIC
OPTIM          := $(OPTIM)
LD             = $(GCC)
LIBTOOL 	   = libtool
LDFLAGS        = -all_load
SOFLAGS        =
DllSuf        := .dylib

ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(ROOTLIBS) -lTreePlayer -lGX11 -lpthread -lThread 
# --new give a runtime error on darwin and root 4.04 :
# <CustomReAlloc2>: passed oldsize 64, should be 0
# Fatal in <CustomReAlloc2>: storage area overwritten
# aborting

ifeq ($(strip $(shell $(FIND) $(BOOST_LIB_DIR) -maxdepth 1 -name libboost_filesystem-mt.so)),$(BOOST_LIB_DIR)/libboost_filesystem-mt.so)
  BOOST_LIBS += -lboost_system-mt
else
  BOOST_LIBS += -lboost_system
endif


endif


############################
############################
# Some set-up for the MySQL library
############################
############################
ifndef MYSQL_INC_DIR
  ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name mysql)),/usr/include/mysql)
    $(warning Install the MySQL client library on your system, or set the environment)
    $(warning variables MYSQL_INC_DIR and MYSQL_LIB_DIR to the directory with)
    $(warning the MySQL headers and libraries, respectively.)
    $(warning See the Qweak Wiki for installation and compilation instructions.)
    $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
    $(warning )
    $(warning Could not find the MySQL library)
  else
    $(info Setting MYSQL_INC_DIR to /usr/include/mysql)
    MYSQL_INC_DIR = /usr/include/mysql
    MYSQL_LIB_DIR = /usr/lib/mysql
  endif
endif
ifdef MYSQL_LIB_DIR
  ifndef MYSQLPP_INC_DIR
    ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name mysql++)),/usr/include/mysql++)
      ifneq ($(strip $(shell $(FIND) /usr/local/include -maxdepth 1 -name mysql++)),/usr/local/include/mysql++)
        $(warning Install the MySQL++ library on your system, or set the environment)
        $(warning variables MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR to the directory with)
        $(warning the MySQL++ headers and libraries, respectively.)
        $(warning See the Qweak Wiki for installation and compilation instructions.)
        $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
        $(warning )
        $(warning Could not find the MySQL++ library)
      else
        $(warning Setting MYSQLPP_INC_DIR to /usr/local/include/mysql++)
        MYSQLPP_INC_DIR = /usr/local/include/mysql++
        MYSQLPP_LIB_DIR = /usr/local/lib
      endif
    else
      $(info Setting MYSQLPP_INC_DIR to /usr/include/mysql++)
      MYSQLPP_INC_DIR = /usr/include/mysql++
      MYSQLPP_LIB_DIR = /usr/lib
    endif
  endif
endif
ifdef MYSQLPP_LIB_DIR
$(info Found the mysql libraries)
MYSQL_INC  = -I${MYSQL_INC_DIR}
MYSQL_LIBS = -L${MYSQL_LIB_DIR} -lmysqlclient
MYSQLPP_INC  = -I${MYSQLPP_INC_DIR}
MYSQLPP_LIBS = -L${MYSQLPP_LIB_DIR} -lmysqlpp
DEFAULTADD += -D__USE_DATABASE__
else
MYSQL_INC  =
MYSQL_LIBS =
MYSQLPP_INC  =
MYSQLPP_LIBS =
endif

############################
############################
# Some set-up for the MySQL++ library
############################
############################
#ifndef MYSQLPP_INC_DIR
#  ifneq ($(strip $(shell $(FIND) /usr/include -maxdepth 1 -name mysql++)),/usr/include/mysql++)
#    ifneq ($(strip $(shell $(FIND) /usr/local/include -maxdepth 1 -name mysql++)),/usr/local/include/mysql++)
#      $(warning Install the MySQL++ library on your system, or set the environment)
#      $(warning variables MYSQLPP_INC_DIR and MYSQLPP_LIB_DIR to the directory with)
#      $(warning the MySQL++ headers and libraries, respectively.)
#      $(warning See the Qweak Wiki for installation and compilation instructions.)
#      $(warning ->   http://qweak.jlab.org/wiki/index.php/Software)
#      $(warning )
#      $(warning Could not find the MySQL++ library)
#    else
#      $(warning Setting MYSQLPP_INC_DIR to /usr/local/include/mysql++)
#      MYSQLPP_INC_DIR = /usr/local/include/mysql++
#      MYSQLPP_LIB_DIR = /usr/local/lib
#    endif
#  else
#    $(info Setting MYSQLPP_INC_DIR to /usr/include/mysql++)
#    MYSQLPP_INC_DIR = /usr/include/mysql++
#    MYSQLPP_LIB_DIR = /usr/lib
#  endif
#endif



############################
############################
# A few fixes :
############################
############################

ifeq ($(strip $(shell $(QWANALYSIS)/SetupFiles/checkrootversion | $(GREP) WARNING | $(SED) 's/\*//g')),WARNING)
$(error Aborting : ROOT version 3.01/02 or later required)
# With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
endif

ifeq ($(strip $(shell $(QWANALYSIS)/SetupFiles/checkrootversion | $(GREP) older)),but older than 3.01/06)
DEFAULTADD += -DTGFILEDIALOG_FIX
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
# With version 3.77 or earlier of make, the message is simply 'missing separator.  Stop.'
endif



############################
############################
# Various 'merged' flags for $(CXX) and co :
############################
############################

INCFLAGS =  $(patsubst %,-I%,$(sort $(dir $(shell $(FIND) $(QWANALYSIS) | $(GREP) '\$(IncSuf)' | $(SED) '/\$(IncSuf)./d' | $(FILTER_OUT_TRASH) | $(INTO_RELATIVE_PATH) |  $(FILTER_OUT_LIBRARYDIR_DEPS)))))
# Qw include paths : /SomePath/QwAnalysis/Analysis/include/Foo.h -> -I./Analysis/include/

INCFLAGS += $(MYSQL_INC) $(MYSQLPP_INC)
INCFLAGS += $(BOOST_INC) -I./

# Necessary for dictionary files where include files are quoted with relative
# path appended (default behaviour for root-cint)

CPPFLAGS = $(INCFLAGS) $(ROOTCFLAGS) $(CODACFLAGS) $(sort $(DEFAULTADD) $(ADD))
# ADD should be contained in DEFAULTADD, but DEFAULTADD may be tempered with...

CXXFLAGS += $(OPTIM) $(DEBUG)


ifneq ($(CXX),CC)
  LDLIBS      += -lstdc++
  LDLIBS      += -lz
endif
LIBS =  -L$(QW_LIB) -lQw
LIBS +=  $(ROOTLIBS) $(ROOTGLIBS) $(CODALIBS)
LIBS +=  $(MYSQL_LIBS) $(MYSQLPP_LIBS)
LIBS +=  $(BOOST_LIBS) $(LDLIBS)

############################
############################
# Miscellaneous
############################
############################

TAB   = '	'# <--- This is a tab character, for clarity in rules
                 # Don't touch !

SPACE = ' '# <--- Space character, for clarity


############################
############################
# Tricky variables (type 1 to prevent premature interpretation of special
# characters)
############################
############################

FILTER_OUT_TRASH    = $(SED) '/~$$/d ; /\#/d ; /JUNK/d'
# FILTER_OUT_TRASH pipes stream and filters out '~', '.#' and '#*#'
# typical editor backup file names

FILTER_OUT_DOXYGEN    = $(SED) '/Doxygen/d'
# FILTER_OUT_DOXYGEN pipes stream and filters out 'Doxygen'
# where the html documentation tree is stored

INTO_RELATIVE_PATH  = $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/$(subst /,xxqqqqqxx,$(QWANALYSIS))/./g' | $(SED) 's/xxqqqqqxx/\//g'
# To be piped in
# The special meaning of '/' in regular expressions is painful
# This pipe is a trick to encapsulate the conversion from $(QWANALYSIS) to .


APPEND_BIN_PATH  = $(SED) 's/\//xxqqqqqxx/g' | $(SED) 's/\([a-z0-9_]* \)/$(subst /,xxqqqqqxx,$(QW_BIN))xxqqqqqxx\1/g' | $(SED) 's/xxqqqqqxx/\//g'
# To be piped in, all letters to lower already...
# The special meaning of '/' in regular expressions is painful...

TO_LINE = $(AWK) 'BEGIN {RS="\\"};{print $$0}' | $(AWK) 'BEGIN {RS=" "};{print $$0}'

ADD_ANTISLASH = $(SED) 's/$$/ \\/'

REMOVE_-D = $(SED) 's/-D//g'

FILTER_OUT_FOREIGN_DEPS =  $(SED) 's/^\([A-Za-z_]\)/\.\/\1/' | $(GREP) "\./"
# To be piped in
# To filter out non Qw include file names generated by $(GCC) -M
# Expects paths to be already relative, but preceded by './' : the call
# to $(SED) corrects potential 'QwROOT/src/Foo.C' into './QwROOT/src/Foo.C'
# After the 3 $(AWK) calls, all piped in names are on disctinct lines
# $(GREP) keeps Qw related lines

FILTER_OUT_LIBRARYDIR_DEPS = $(SED) '$(patsubst %,/^.\/%/d;,$(EXCLUDEDIRS))'

#PROCESS_GCC_ERROR_MESSAGE =  $(SED) 's/In file/xxqqqqqxx/' | $(AWK) 'BEGIN {RS="qqqxx"};{print $$0}' | $(SED) '/included/d'
# Obsolete : needed On_ONE_LINE, but incompatible with SunOS
# To be piped in
# 'In file' is first words of the error message when $(GCC) -MM failed
# Encapsulate controled removal of this error message and leaves xxqq tags


# ON_ONE_LINE = $(SED) 's/$$/ \\/' | $(SED) ':b;/\\$$/{;N;s/\\\n//;bb;}'
# Incompatible with 'sed' on SunOS
# To be piped in
# Gets everything on one line

############################
############################
# Main targets :
############################
############################

export

all: .ADD .EXES QwSVNVersion .auxDepends bin/qweak-config
ifneq ($(strip $(ADD)),)
	@if [ "$(strip $(sort $(shell $(CAT) .ADD)))" != "$(strip $(sort $(ADD)))" ]; \
	then \
	$(ECHO) ; \
	$(ECHO) \*\*\* ADD options have changed since last config; \
	$(ECHO) \*\*\* Removing involved object files... ; \
	$(ECHO) \*\*\* Rerun \'make config\' with your new options; \
	$(ECHO) \*\*\* Then rerun \'make\' \(you can omit your new options\); \
	$(ECHO) ; \
	for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .ADD),$(ADD)) $(filter-out $(ADD),$(shell $(CAT) .ADD)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(GREP) $$wd */*/*$(IncSuf) */*/*$(SrcSuf) | $(SED) 's/^\([A-Za-z0-9\/\._]*\):.*/\1/g;s/\$(IncSuf)/\$(ObjSuf)/g;s/\$(SrcSuf)/\$(ObjSuf)/g'`; \
	done; \
	exit 1; \
	fi
endif
ifneq ($(strip $(EXES)),)
	@if [ "$(strip $(sort $(shell $(CAT) .EXES)))" != "$(strip $(sort $(EXES)))" ]; \
	then \
	$(ECHO) ; \
	$(ECHO) \*\*\* EXES choice has changed since last config; \
	$(ECHO) \*\*\* Removing involved object files... ; \
	$(ECHO) \*\*\* Rerun \'make config\' with your new options; \
	$(ECHO) \*\*\* Then rerun \'make\' \(you can omit your new options\); \
	$(ECHO) ; \
	for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .EXES),$(EXES)) $(filter-out $(EXES),$(shell $(CAT) .EXES)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(CAT) .auxDepends | $(SED) "/$$wd/!d;s/.*$$wd: \([A-Za-z0-9\/\._]*\$(ObjSuf)\) .*/\1/"` $(QW_LIB)/libQw$(DllSuf); \
	done; \
	exit 1; \
	fi
endif
	@$(MAKE) myevio_lib
	@$(MAKE) -f .auxDepends `$(CAT) .auxExeFiles | $(SED) 's/$$/ /g' | $(APPEND_BIN_PATH) | $(INTO_RELATIVE_PATH)`


config: .ADD .EXES clean.auxfiles QwSVNVersion .auxDepends bin/qweak-config 
	@for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .ADD),$(ADD)) $(filter-out $(ADD),$(shell $(CAT) .ADD)) | $(REMOVE_-D))); \
	do \
	$(RM) `$(GREP) $$wd */*/*$(IncSuf) */*/*$(SrcSuf) | $(SED) 's/^\([A-Za-z0-9\/\._]*\):.*/\1/g;s/\$(IncSuf)/\$(ObjSuf)/g;s/\$(SrcSuf)/\$(ObjSuf)/g'`; \
	done
	@for wd in xxxdummyxxx $(sort $(shell $(ECHO) $(filter-out $(shell $(CAT) .EXES),$(EXES)) $(filter-out $(EXES),$(shell $(CAT) .EXES)) | $(REMOVE_-D))); \
	do \
	cd $(QW_BIN);$(RM) `$(LS) $(QW_BIN) | $(GREP) -v 'qweak-config' | $(SED) 's/^.*\.pl$$//g' | $(SED) 's/CVS//g' | $(SED) 's/SunWS_cache//g'` $(QW_LIB)/libQw$(DllSuf); \
	done
	@$(ECHO) $(ADD)  | $(TO_LINE) > .ADD
	@$(ECHO) $(EXES)  | $(TO_LINE) > .EXES

myevio_lib:
	$(MAKE) -C $(EVIO) libmyevio$(DllSuf)
	$(CP) -p $(EVIO)/libmyevio$(DllSuf) $(QW_LIB)/libmyevio$(DllSuf) 

.auxDepends: .auxLibFiles
	@$(ECHO) Generating .auxLibFiles
	@$(RM) .auxLibFiles
	@$(ECHO) $(QW_LIB)/libQw$(DllSuf) | $(INTO_RELATIVE_PATH) > .auxLibFiles
	@$(ECHO) $(QW_LIB)/libQw$(DllSuf): `$(CAT) .auxSrcFiles` `$(CAT) .auxDictFiles` \
		| $(TO_LINE) \
		| $(INTO_RELATIVE_PATH) \
		| $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/g' \
		| $(ADD_ANTISLASH) \
		| $(FILTER_OUT_FOREIGN_DEPS) >> .auxDepends
	@$(ECHO) >> .auxDepends
	@$(ECHO) $(TAB)$(LIBTOOL) $(if $(findstring $(CXX),$(LIBTOOL)),$(CXXFLAGS) $(SOFLAGS) $(LDFLAGS)) '$$^' -o $(QW_LIB)/libQw$(DllSuf) | $(INTO_RELATIVE_PATH) >> .auxDepends
	@$(ECHO) $(TAB)@$(ECHO) >> .auxDepends
	@$(ECHO) >> .auxDepends
	@for file in `$(CAT) 2>&1 .auxMainFiles`; \
	do \
	$(ECHO) $(QW_BIN)/`$(ECHO) $$file | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'`: `$(ECHO) $$file | $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/'` `$(CAT) .auxLibFiles` $(QW_LIB)/libmyevio$(DllSuf)  | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	$(ECHO) $(TAB)$(LD) $(CXXFLAGS) '$$<' $(LDFLAGS) $(LIBS) -o '$$@' | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done


.auxLibFiles: .auxLinkDefFiles
	@for file in `$(CAT) 2>&1 .auxDictFiles`; \
	do \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) $$file | $(SED) 's/\(.*\/\)dictionary\(\/.*\)Dict\$(SrcSuf)/&: \1include\2\$(IncSuf)/' >> .auxDepends; \
	$(ECHO) $(TAB)@$(ROOTCINT) -f '$$@' -c -p $(INCFLAGS) $(CODACFLAGS) -DROOTCINTFIX "\`""$(CAT) .auxLinkDefFiles | $(GREP)" '$$<'"\`" >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) $(ROOTCINT) -f '$$@' -c $(INCFLAGS) $(CODACFLAGS) -DROOTCINTFIX "\`""$(CAT) .auxLinkDefFiles | $(GREP)" '$$<'"\`" >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done
	@for file in `$(CAT) 2>&1 .auxDictFiles | $(SED) 's/\$(SrcSuf)/\$(ObjSuf)/'`; \
	do \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) $$file | $(SED) 's/\(\.[A-Za-z0-9\/_]*\)\$(ObjSuf)/&: \1\$(SrcSuf)/' >> .auxDepends; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .auxDepends; \
	$(ECHO) $(TAB)@$(ECHO) >> .auxDepends; \
	$(ECHO) >> .auxDepends; \
	done


.auxLinkDefFiles : .auxDictFiles
	@$(RM) .tmp1 .tmp2
	@$(ECHO) Generating $@
	@$(FIND) $(QWANALYSIS) | $(GREP) '\$(IncSuf)' | $(INTO_RELATIVE_PATH) | $(FILTER_OUT_LIBRARYDIR_DEPS) | $(FILTER_OUT_DOXYGEN) | $(SED) '\@/\.svn/@d; /LinkDef/d;/Dict/d;s/\$(IncSuf)//' > .tmp1
	@$(FIND) $(QWANALYSIS) | $(GREP) LinkDef | $(INTO_RELATIVE_PATH) | $(FILTER_OUT_LIBRARYDIR_DEPS) | $(FILTER_OUT_DOXYGEN) | $(SED) '\@/\.svn/@d;s/LinkDef\$(IncSuf)//'> .tmp2
	@for file in `$(CAT) .tmp1`; \
	do \
	if [ "`$(GREP) $$file .tmp2`" != "" ]; \
	then \
	$(ECHO) $$file$(IncSuf) $$file\LinkDef$(IncSuf) >> $@; \
	else \
	$(ECHO) $$file$(IncSuf) >> $@; \
	fi; \
	done
	@$(RM) .tmp1 .tmp2


.auxDictFiles: .auxSrcFiles
	@stem () { \
	$(BASENAME) $$1 | $(SED) 's/\$(SrcSuf)//'; \
	}; \
	$(RM) .aux; \
	for file in `$(CAT) 2>&1 .auxSrcFiles`; \
	do \
	$(RM) .tmp; \
	$(RM) .tmperror; \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) `$(DIRNAME) $$file`/`$(GCC) $(CPPFLAGS) 2>.tmperror -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(ADD_ANTISLASH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	>> .tmp; \
	if [ -s .tmperror ];\
	then \
	$(ECHO) "Aborting:  Unable to locate a file in the include paths:"; \
	$(CAT) .tmperror; \
	exit 1; \
	fi;\
	$(RM) .tmperror; \
	$(CAT) .tmp | $(GREP) -v "\/$$(stem $$file)\." | $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" >> .aux; \
	if [ "`$(CAT) .tmp | $(GREP) /In | $(SED) '/In[A-Za-z0-9\/_]/d'`" = "" ]; \
	then \
	$(ECHO)  >> .tmp; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .tmp; \
	$(ECHO) $(TAB)@$(ECHO)>> .tmp; \
	$(ECHO) >> .tmp; \
	$(CAT) .tmp | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	fi; \
	done; \
	$(RM) .auxSrcFiles; \
	$(TOUCH) .auxSrcFiles; \
	$(ECHO) Checking for nested dependencies...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	$(ECHO) `$(GCC) $(CPPFLAGS) 2>.tmperror -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	| $(GREP) -v "\/$$(stem $$file)\." \
	| $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" \
	>> .aux; \
	if [ -s .tmperror ];\
	then \
	$(ECHO) "Aborting:  Unable to locate a file in the include paths:"; \
	$(CAT) .tmperror; \
	exit 1; \
	fi;\
	$(RM) .tmperror; \
	fi; \
	fi; \
	done; \
	$(ECHO) Checking for double-nested dependencies...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	$(ECHO) `$(GCC) $(CPPFLAGS) 2>&1 -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	| $(GREP) -v "\/$$(stem $$file)\." \
	| $(SED) "s/\/include\//\/src\//g;s/\$(IncSuf)/\$(SrcSuf)/g;s/\\\//g" \
	>> .aux; \
	fi; \
	fi; \
	done; \
	$(ECHO) Building list of source files...; \
	for file in `$(CAT) 2>&1 .aux`; \
	do \
	if [ "`$(GREP) $$file .auxSrcFiles`" = "" ]; \
	then \
	if [ -f  $$file ]; \
	then \
	$(ECHO) $$file | $(FILTER_OUT_LIBRARYDIR_DEPS) >> .auxSrcFiles; \
	fi; \
	fi; \
	done
	@for file in `$(CAT) 2>&1 .auxSrcFiles`; \
	do \
	$(RM) .tmp; \
	$(ECHO) Writing dependencies for file $$file...; \
	$(ECHO) `$(DIRNAME) $$file`/`$(GCC) $(CPPFLAGS) 2>&1 -MM $$file` \
	| $(TO_LINE) \
	| $(INTO_RELATIVE_PATH) \
	| $(ADD_ANTISLASH) \
	| $(FILTER_OUT_FOREIGN_DEPS) \
	>> .tmp; \
	if [ "`$(CAT) .tmp | $(GREP) /In | $(SED) '/In[A-Za-z0-9\/_]/d'`" = "" ]; \
	then \
	$(ECHO)  >> .tmp; \
	$(ECHO) $(TAB)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o '$$@' -c '$$<' >> .tmp; \
	$(ECHO) $(TAB)@$(ECHO)>> .tmp; \
	$(ECHO) >> .tmp; \
	$(CAT) .tmp | $(INTO_RELATIVE_PATH) >> .auxDepends; \
	fi; \
	done;
	@$(RM) .tmp .aux
	@$(ECHO) Generating .auxDictFiles
	@$(RM) .auxDictFiles
	@$(TOUCH) .auxDictFiles
	@for file in `$(CAT) .auxDepends | $(SED) '/:/!d;s/\(.*\):\(.*\)/\1/;s/\$(ObjSuf)/\$(SrcSuf)/'`; \
	do \
	if [ "`$(GREP) 'ClassImp' $$file | $(SED) '/^ClassImp/!d;s/\(^ClassImp(\)\(.*\)\()\)/\2/'`" != "" ]; \
	then \
	$(ECHO) $$file | $(SED) 's/\(.*\/\)src\(\/.*\)\$(SrcSuf)/\1dictionary\2Dict\$(SrcSuf)/' | $(INTO_RELATIVE_PATH) >> .auxDictFiles; \
	fi; \
	done



.auxSrcFiles: .auxExeFiles
	@$(ECHO) Generating $@
	@for file in $(shell $(FIND) $(QWANALYSIS) | $(SED) '/\/main\//!d;\@/\.svn/@d;/CVS/d;/\$(SrcSuf)/!d' | $(FILTER_OUT_TRASH)); \
	do \
	case `$(ECHO) $$file | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'` in ${foreach exe,$(shell $(CAT) $<),$(exe) ${shell ${ECHO} ")"} $(ECHO) $$file | $(INTO_RELATIVE_PATH) >> $@;;} \
	esac; \
	done
	@$(CAT) .auxSrcFiles > .auxMainFiles


.auxExeFiles:
	@$(ECHO) Generating $@
#	$(FIND) $(QWANALYSIS) | $(SED) '/\/main\//!d;\@/\.svn/@d;/CVS/d;/\$(SrcSuf)/!d' | $(FILTER_OUT_TRASH) | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'
	@$(ECHO) $(filter $(shell $(FIND) $(QWANALYSIS) | $(SED) '/\/main\//!d;\@/\.svn/@d;/CVS/d;/\$(SrcSuf)/!d' | $(FILTER_OUT_TRASH) | $(SED) 's/.*\/\([A-Za-z0-9_]*\)\$(SrcSuf)/\1/;y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/'),$(EXES)) > $@


.ADD:
	@$(ECHO) Generating $@
	@$(ECHO) $(ADD)  | $(TO_LINE) > .ADD

.EXES:
	@$(ECHO) Generating $@
	@$(ECHO) $(EXES)  | $(TO_LINE) > .EXES

bin/qweak-config: qweak-config.in
	@$(ECHO) Generating $@
	@$(CAT) $< | $(SED) 's!%QWANALYSIS%!$(QWANALYSIS)!' | $(SED) 's!%LIBS%!$(LIBS)!'   \
	           | $(SED) 's!%QW_LIB%!$(QW_LIB)!' | $(SED) 's!%QW_BIN%!$(QW_BIN)!'           \
	           | $(SED) 's!%LDFLAGS%!$(LDFLAGS)!' | $(SED) 's!%CPPFLAGS%!$(CPPFLAGS)!' \
	           > $@
	@$(CHMOD) a+x $@


QwSVNVersion:
	@if [ "$(QWANA_SVN_REVISION)" != "$(QWANA_SVN_REVISION_OLD)" ] ; \
	then \
		$(ECHO) "Generating $(QWANA_VERSION_H) with revision string, $(QWANA_SVN_REVISION)" ; \
		$(ECHO) '#define QWANA_SVN_REVISION "'$(QWANA_SVN_REVISION)'"' > $(QWANA_VERSION_H); \
		$(ECHO) '#define QWANA_SVN_URL "'$(QWANA_SVN_URL)'"' >> $(QWANA_VERSION_H); \
		$(ECHO) '#define QWANA_SVN_LASTCHANGEDREVISION "'$(QWANA_SVN_LASTREVISION)'"' >> $(QWANA_VERSION_H); \
	else \
		$(ECHO) "\`$(QWANA_VERSION_H)' is up to date"; \
	fi;

EXTENSIONS = Extensions/EventDisplay3D Extensions/GUI Extensions/Regression/QwBeamModulation

extensions: $(EXTENSIONS)

$(EXTENSIONS): .auxExeFiles
	$(MAKE) -C $@

.PHONY: extensions $(EXTENSIONS)

############################
############################
# Specific clean targets :
############################
############################

clean.auxfiles:
# Removes auxiliary config files
	@$(ECHO) Removing '.aux*' files
	@$(RM) .aux*


clean.dictfiles:
# Removes all dict files '*Dict*'
	@$(ECHO) Removing *Dict$(SrcSuf), *Dict$(IncSuf) files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) 'Dict\$(SrcSuf)' | $(SED) '/\$(SrcSuf)./d'`
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) 'Dict\$(IncSuf)' | $(SED) '/\$(IncSuf)./d'`
	@if [ -e $(QWANA_VERSION_H) ] ;\
	then \
		$(ECHO) Removing $(QWANA_VERSION_H); \
		$(RM) $(QWANA_VERSION_H); \
	fi;


clean.libs:
# Removes libraries
	@$(ECHO) Removing '*$(DllSuf)' files
	@$(RM) $(QW_LIB)/lib*$(DllSuf)


clean: clean.evio
# Removes all object files, '*~', '#*#' and '.#*' files
	@$(ECHO) Removing '*$(ObjSuf)' files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '\$(ObjSuf)' | $(SED) '/\$(ObjSuf)./d'`
	@$(ECHO) Removing *~, "#*#, .#*" files
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '~'`
	@$(RM) `$(FIND) $(QWANALYSIS) | $(GREP) '#'`

clean.exes:
# Removes executables
	@$(ECHO) Removing executables
	@$(RM) `$(LS) $(QW_BIN)/* | $(SED) 's/^.*\.pl$$//g' | $(SED) 's/^.*\.sh$$//g' | $(SED) 's/[A-Za-z0-9\/_]*CVS//'`

clean.olddotfiles:
	@$(RM) .dirs .libdepend .libdepend2 .exedepend .exedepend2 .mains .dictdepend .exes .objdepend .dicts .incdirs .srcdirs $(SETUP)/.MapFileBaseAddress



cleanSunWS_cache :
	@$(RM) -r $(filter %SunWS_cache/,$(sort $(dir $(shell $(FIND) $(QWANALYSIS)))))
	@$(RM) -r $(filter %SunWS_cache,$(sort $(shell $(FIND) $(QWANALYSIS))))

clean.evio:
	cd $(EVIO); $(MAKE) realclean

distclean: cleanSunWS_cache clean.dictfiles clean clean.libs clean.exes clean.auxfiles clean.olddotfiles clean.evio
# Removes all files that can be regenerated
	@$(RM) .ADD .EXES



############################
############################
# Built-in targets :
############################
############################

.PHONY : clean clean.dictfiles clean.exes clean.libs distclean clean.auxfiles config all clean.olddotfiles cleanSunWS_cache QwSVNVersion

.SUFFIXES :
.SUFFIXES : $(SrcSuf) $(IncSuf) $(ObjSuf)

