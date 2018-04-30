#!/bin/csh
#
# QWEAK environment variables ((t)csh)
#
#

unsetenv OSNAME
setenv OSNAME `uname|sed 's/-/_/'`


if (${OSNAME} != "Linux" && ${OSNAME} != "SunOS" && ${OSNAME} != "OSF1" && ${OSNAME} != "Darwin") then
    echo "Operating system name '"${OSNAME}"' does not have a version of the QWEAK analyzer."
    exit
endif

echo Initializing QWEAK analysis
#echo "SHELL: " $SHELL
echo "OSNAME: " $OSNAME

##if ($?RCPATH) then
##  unsetenv PATH
##  echo Recovering initial value of PATH : $RCPATH
##  setenv PATH $RCPATH
##endif
##if (! $?RCPATH && $?PATH) then
##  echo Backing variable PATH into RCPATH for future references
##  setenv RCPATH $PATH
##endif
##
##if ($?RCLD_LIBRARY_PATH) then
##  unsetenv LD_LIBRARY_PATH
##  echo Recovering initial value of LD_LIBRARY_PATH : $RCLD_LIBRARY_PATH
##  setenv LD_LIBRARY_PATH $RCLD_LIBRARY_PATH
##endif
##if (! $?RCLD_LIBRARY_PATH && $?LD_LIBRARY_PATH) then
##  echo Backing variable LD_LIBRARY_PATH into RCLD_LIBRARY_PATH for future references
##  setenv RCLD_LIBRARY_PATH $LD_LIBRARY_PATH
##endif


if ($?QWANALYSIS && ! $?QWVERSION) then
  echo QWANALYSIS already defined : ${QWANALYSIS}
else
  echo Either QWANALYSIS not defined in .cshrc file or QWVERSION defined
  echo Expecting to be on site : autosetting variable QWANALYSIS to group disc
  unsetenv QWANALYSIS
  if ($?QWVERSION) then
    echo "QWVERSION: " $QWVERSION
    setenv QWANALYSIS          /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis_${QWVERSION}
    if (! -d ${QWANALYSIS})  then
      echo "Version '"$QWVERSION"' is not found; trying standard version."
      setenv QWANALYSIS        /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
    endif
  else
    echo The variable QWVERSION is not set; trying standard version.
    setenv QWANALYSIS          /group/qweak/QwAnalysis/${OSNAME}/QwAnalysis
  endif
  echo "Setting QWANALYSIS to " ${QWANALYSIS}
endif



###  Set up the analyzer and scratch environments.
source ${QWANALYSIS}/SetupFiles/make_scratch_dir
source ${QWANALYSIS}/SetupFiles/.Qwcshrc
source ${QWANALYSIS}/SetupFiles/.Qwcshrc_scratch


