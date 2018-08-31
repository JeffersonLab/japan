
##############
# Environment :
##############

SHELL     = /bin/sh
ARCH      = $(shell uname)
MAKE      = $(word $(words $(shell which gmake)),$(shell which gmake))
# system dependent, but '/usr/bin/gmake' is incorrect on some system)
# the trick is to get rid of any verbose parts that precedes the return
AWK       = gawk
BASENAME := basename
CAT      := cat
CD       := cd
CHMOD    := chmod
DIRNAME  := dirname
ECHO     := echo
FIND     := find
GCC      := gcc
      # This must be the GNU compiler collection : explicit use of
      # flag '-M' for automatic search for dependencies
      # It is not correlated to $(CXX) and $(LD) which depend on $(ARCH)
GREP     := grep
LS       := ls
MAKE     := make
RM-f     := \rm -f
RM-rf    := \rm -rf
      # 'rm' is often aliases as 'rm -i', so '\rm' instead
MV       := \mv
CP       := \cp
ROOTCINT := rootcint
SED      := sed -e
TOUCH    := touch
CXX       =

#### VPATH	  = $QWANALYSIS/lib:$QWANALYSIS/Analysis/include:$QWANALYSIS/Analysis/src:$QWANALYSIS/Parity/include:$QWANALYSIS/Parity/src



MYSRCDIR = ./src
MYINCDIR = ./include

VPATH    = ./main:./src




ifndef CODA
    $(error   Error: The CODA variable is not defined; the feedback analyzer requires linking against a complete CODA installation.)
endif

##################
##################
###  Get the flags from the QWANALYSIS build
##################

QWEAKLIBS     := $(shell ${QWANALYSIS}/bin/qweak-config --libs)
QWEAKLDFLAGS  := $(shell ${QWANALYSIS}/bin/qweak-config --ldflags)
#  When we get the QwAnalysis include paths, turn paths like "./*" into "${QWANALYSIS}/*".
REGEXP_QWANA  := $(shell ${ECHO} ${QWANALYSIS} | ${SED} 's/\//\\\//g')
QWEAKCPPFLAGS := $(shell ${QWANALYSIS}/bin/qweak-config --cppflags | sed 's/I\./I${REGEXP_QWANA}/g' )

##################

ifeq ($(ARCH),Linux)
  CXX        =  g++
  LINKER     =  g++
  CFLAGS     =  -Wall -fPIC -O3
  ###CFLAGS     =  -Wall -fPIC
endif

ifeq ($(ARCH),SunOS)
  CXX        = CC
  LINKER     = CC
  CFLAGS     = -KPIC
  CLIBS      = -lm -lposix4  -lsocket -lnsl -lresolv -ldl 
endif

#####


##################
# JLab EPICS libs
##################
ifdef EPICS
  EPICSFLAGS   := -I$(EPICS)/base/include -I$(EPICS)/base/include/os/Linux
  # On the cdaq cluster, the library directory is different between the 3.13
  # and 3.14 EPICS versions
  ifeq ($(shell $(AWK) '$$2="EPICS_REVISION" {print $$3}' $(EPICS)/base/include/epicsVersion.h),13)
    EPICSLIBS    := -L${EPICS}/base/lib/Linux -lca -lcas -lCom -lAs -lgdd
  else
    EPICSLIBS    := -L${EPICS}/base/lib/linux-x86 -lca -lcas -lCom -lgdd
  endif
else
  $(error Aborting : EPICS library is not accessible from the system)
endif

##############
##############
##############

CFLAGS   += $(EPICSFLAGS) -I${MYINCDIR} 
CPPFLAGS  = $(QWEAKCPPFLAGS)
LDFLAGS   = $(QWEAKLDFLAGS)
LIBS      = $(EPICSLIBS) $(QWEAKLIBS)  -L$(QW_LIB) -lQw

##############
##############
##############

MYSRCFILES= $(shell find $(MYSRCDIR) -name \*.cc -or -name \*.c)
MYINCFILES= $(shell find $(MYINCDIR) -name \*.h)

MYOBJFILES= $(shell echo $(MYSRCFILES)|sed -e 's/\.cc/\.o/g' -e 's/\.c/\.o/g')

##############

default : qwfeedback 
all:  default

qwfeedback :  main/QwFeedback.o $(MYOBJFILES)
	@echo
	@echo
	@echo
	@echo
	@echo  "########## Making \"qwfeedback\"  ##########"
	@echo  Local source files used: $(MYOBJFILES).
	@echo  Local include files used: $(MYINCFILES).
	@echo  "########## Making \"qwfeedback\"  ##########"> .lastbuild-qwfeedback
	@echo  $(shell date) >> .lastbuild-qwfeedback
	@echo  Local source files used: $(MYOBJFILES). >>.lastbuild-qwfeedback
	@echo  Local include files used: $(MYINCFILES).>>.lastbuild-qwfeedback
	@echo
	${LINKER} ${CFLAGS}  ${filter-out %.a %.so, $^}  ${LDFLAGS} ${LIBS} -o  $@   
	@echo
	@echo  "###### Finished making \"qwfeedback\" ######"
	@echo  "###### Finished making \"qwfeedback\" ######" >>.lastbuild-qwfeedback
	@echo

clean:
	rm -f *.o */*.o core qwfeedback *~ */*~

main/%.o: main/%.cc $(MYINCFILES)
	@echo  "######"
	$(CXX)  ${CFLAGS}  ${CPPFLAGS} -o $@ -c $<
	@echo  "######"

src/%.o: src/%.cc include/%.h
	@echo  "######" 
	$(CXX)  ${CFLAGS}  ${CPPFLAGS} -o $@ -c $< 
	@echo  "######"

src/%.o: src/%.c include/%.h
	@echo  "######" 
	$(GCC)  ${CFLAGS} -o $@ -c $< 
	@echo  "######"

%.o: %.cc 
	${CXX} ${CFLAGS} ${CPPFLAGS}  -c $< 
	@echo  "######"





