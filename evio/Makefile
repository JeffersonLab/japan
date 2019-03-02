# Makefile for CODA classes
# R. Michaels, rom@jlab.org, Feb, 2001
# Updated evio library, Oct 19, 2001
#
# Test Executibles:
# tstcoda  --  test of File or ET connection.
# etclient --  test of ET connection for online data.
#
#
# All the root stuff could be discarded (with a little surgery
# on the sources), but I leave it in the Makefile, which was 
# taken from another project.
#
# Note regarding software from DAQ group
# evio.h, evio.C, swap_util.C, et.h are taken from DAQ group intact
# (only a few tiny mod's by me).  For this reason I did not rename 
# those modules with prefix "THa". 
#
# You need an environment variable to define ROOT. E.g.
#   ROOTSYS = /apps/root/2.25-03/root
# User must have LD_LIBRARY_PATH = $ROOTSYS/lib:$LD_LIBRARY_PATH
#
# Use this if compiling online code (ET system)
# User must have LD_LIBRARY_PATH = $CODA/$OSNAME/lib:$LD_LIBRARY_PATH
# export ONLINE = 1

# Use this if profiling (note: it slows down the code)
# export PROFILE = 1

# To make standalone, independent of root CINT macros
export STANDALONE = 1

export OSNAME := $(shell uname)

MYSRCDIR = src
MYINCDIR = include

MYINCLUDES = -I$(MYINCDIR)

ifdef CODA
export ONLINE = 1
endif

ifeq ($(OSNAME),Darwin)

   ROOTCFLAGS    = $(shell root-config --cflags)
   ROOTLIBS      = $(shell root-config --libs)
   ROOTGLIBS     = $(shell root-config --glibs)
   CXX           = g++
#   CXXFLAGS      = -O  -DSUNVERS -I$(ROOTSYS)/root/include
   CXXFLAGS      = -O  -DSUNVERS 
   LD            = $(CXX)
   LDFLAGS       = -g -D
   SOFLAGS       = -dynamiclib
   CXXFLAGS     += $(ROOTCFLAGS)
   EVIO_LIB=libevio.a
   GLIB =  -lm -lc -lgen -lw -lsocket -lnsl -ldl
#    SLIB = -L/opt/SUNWspro/SC4.2/lib -lF77 -lM77 -lsunmath
# all this needed for ET...  up to comment below
#    ET_AC_FLAGS = -D_REENTRANT -D_POSIX_THREAD_SEMANTICS
#    ET_CFLAGS = -mt -fast -xO5  $(ET_AC_FLAGS) -DSUNVERS
#    CODA = /adaqfs/halla/apar/coda/2.2.1
#    LIBET = $(CODA)/SunOS/lib/libet.so
#    ONLIBS = $(LIBET) -lposix4 -lnsl -lsocket -lresolv
#    LIBS = $(EVIO_LIB) $(GLIB)
   ifdef ONLINE
     CXXFLAGS += -I$(CODA)/common/include
     ALL_LIBS = $(LIBS) $(ONLIBS) 
# ... finished, what ET needs.
   else
     ALL_LIBS = $(LIBS) 
   endif
   ALL_LIBS += $(ROOTLIBS) $(ROOTGLIBS)

endif

ifeq ($(OSNAME),SunOS)

   ROOTCFLAGS    = $(shell root-config --cflags)
   ROOTLIBS      = $(shell root-config --libs)
   ROOTGLIBS     = $(shell root-config --glibs)
   CXX           = CC
#   CXXFLAGS      = -O  -DSUNVERS -I$(ROOTSYS)/root/include
   CXXFLAGS      = -O  -DSUNVERS 
   LD            = CC
   LDFLAGS       = -g -D
   SOFLAGS       = -G
   CXXFLAGS     += $(ROOTCFLAGS)
   EVIO_LIB=libevio.a
   GLIB =  -lm -lc -lgen -lw -lsocket -lnsl -ldl
   SLIB = -L/opt/SUNWspro/SC4.2/lib -lF77 -lM77 -lsunmath
# all this needed for ET...  up to comment below
   ET_AC_FLAGS = -D_REENTRANT -D_POSIX_THREAD_SEMANTICS
   ET_CFLAGS = -mt -fast -xO5  $(ET_AC_FLAGS) -DSUNVERS
   CODA = /adaqfs/halla/apar/coda/2.2.1
   LIBET = $(CODA)/SunOS/lib/libet.so
   ONLIBS = $(LIBET) -lposix4 -lnsl -lsocket -lresolv
   LIBS = $(EVIO_LIB) $(GLIB)
   ifdef ONLINE
     CXXFLAGS += -I$(CODA)/common/include
     ALL_LIBS = $(LIBS) $(ONLIBS) 
# ... finished, what ET needs.
   else
     ALL_LIBS = $(LIBS) 
   endif
   ALL_LIBS += $(ROOTLIBS) $(ROOTGLIBS)

endif


ifeq ($(OSNAME),Linux)

   ifdef OLDROOT
     ROOTLIBS      = -L$(ROOTSYS)/lib -lNew -lBase -lCint -lClib -lCont -lFunc \
                    -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
                    -lPhysics -lPostscript -lProof -lRint -lTree -lUnix -lZip
     ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d
   else
     ROOTCFLAGS    = $(shell root-config --cflags)
     ROOTLIBS      = $(shell root-config --libs)
     ROOTGLIBS     = $(shell root-config --glibs)
   endif

   INCLUDES      = -I$(shell root-config --incdir)
   CXX           = g++
   CXXFLAGS      = -O2 -Wall -fno-exceptions -fPIC $(ROOTCFLAGS) $(INCLUDES)
   LD            = g++
   LDFLAGS       = 
   SOFLAGS       = -shared

   ifdef OLDROOT
      LIBS          = $(ROOTLIBS) -lm -ldl -rdynamic
      GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -L/usr/X11R6/lib \
                      -lXpm -lX11 -lm -ldl -rdynamic
      CXXFLAGS     += -DOLDROOT
   else
      LIBS          = $(ROOTLIBS)
      GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11
   endif

   EVIO_LIB=libevio.a
   ALL_LIBS = $(EVIO_LIB) $(GLIBS) $(ROOTLIBS) 

# ONLIBS is needed for ET
  ET_AC_FLAGS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
  ET_CFLAGS = -02 -fPIC $(ET_AC_FLAGS) -DLINUXVERS
# CODA may be an environment variable.  Typical examples
#  CODA = /adaqfs/coda/2.2
#  CODA = /data7/user/coda/2.2
  LIBET = $(CODA)/Linux/lib/libet.so
  ONLIBS = $(LIBET) -lieee -lpthread -ldl -lresolv

  ifdef ONLINE
     CXXFLAGS += -I$(CODA)/common/include
     ALL_LIBS += $(ONLIBS)
  endif

  ifdef PROFILE
     CXXFLAGS += -pg
  endif

endif

CXXFLAGS += $(MYINCLUDES)

ifdef CODA
SRC = THaEtClient.C THaCodaFile.C THaCodaData.C
ALL = etclient etHeartbeat tstio tstcoda libevio.a libmyevio.a libmyevio.so
else
SRC = THaCodaFile.C THaCodaData.C 
ALL = tstio libevio.a libmyevio.a libmyevio.so
endif
HEAD = $(SRC:.C=.h)
DEPS = $(SRC:.C=.d)
DECODE_OBJS = $(SRC:.C=.o)


ifdef STANDALONE 
  CXXFLAGS += -DSTANDALONE
endif

all: $(ALL)

etclient: etclient_main.o THaEtClient.o THaCodaData.o $(MYINCDIR)/THaEtClient.h $(MYINCDIR)/THaCodaData.h libevio.a
	$(CXX) $(CXXFLAGS) -o $@ etclient_main.o THaEtClient.o THaCodaData.o $(ALL_LIBS) 

etHeartbeat: etHeartbeat_main.o THaEtClient.o THaCodaData.o $(MYINCDIR)/THaEtClient.h $(MYINCDIR)/THaCodaData.h libevio.a
	$(CXX) $(CXXFLAGS) -o $@ etHeartbeat_main.o THaEtClient.o THaCodaData.o $(ALL_LIBS) 

tstio: tstio_main.o THaCodaFile.o THaCodaData.o $(MYINCDIR)/THaCodaFile.h $(MYINCDIR)/THaCodaData.h libevio.a
	$(CXX) $(CXXFLAGS) -o $@ tstio_main.o THaCodaFile.o THaCodaData.o $(ALL_LIBS) 

tstcoda: tstcoda_main.o THaCodaFile.o THaCodaData.o THaEtClient.o $(MYINCDIR)/THaCodaFile.h $(MYINCDIR)/THaCodaData.h $(MYINCDIR)/THaEtClient.h libevio.a
	$(CXX) $(CXXFLAGS) -o $@ tstcoda_main.o THaEtClient.o THaCodaFile.o THaCodaData.o $(ALL_LIBS) 


# Here we build a library with all this stuff
libmyevio.a: $(DECODE_OBJS) evio.o swap_util.o 
	rm -f $@
	ar cr $@ $(DECODE_OBJS) evio.o swap_util.o 

# Here we build a library with all this stuff
libmyevio.so: $(DECODE_OBJS) evio.o swap_util.o 
	rm -f $@
	$(LD) $(ROOTLIBS) $(SOFLAGS) -o $@ $(DECODE_OBJS) evio.o swap_util.o 

%.dylib : %.a
	cp $< $@
	ranlib $@

# Below is the evio library, which comes rather directly 
# from CODA group with minor tweaking by R. Michaels & O. Hansen.

libevio.a: clean_evio evio.o swap_util.o 
	rm -f $@
	ar cr $@ evio.o swap_util.o 

#evio.o: $(MYSRCDIR)/evio.C
#	$(CXX) -fPIC $(MYINCLUDES) -c  $<

#swap_util.o: $(MYSRCDIR)/swap_util.C
#	$(CXX) -fPIC $(MYINCLUDES) -c  $<

clean:  clean_evio
	rm -f *.o *.a *.so core *~ *.d *.out *.tar $(ALL)

realclean:  clean
	rm -f *.d

clean_evio:
	rm -f evio.o swap_util.o 


.SUFFIXES:
.SUFFIXES: .c .cc .cpp .C .o .d

%.o:	$(MYSRCDIR)/%.C
	$(CXX) $(CXXFLAGS) -c $<

%.d:	$(MYSRCDIR)/%.C
	@echo Creating dependencies for $<
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) -c $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

-include $(DEPS)








