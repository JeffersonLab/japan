setenv PATH `echo $PATH | sed 's/:/\n/g' | grep -v "5.34.36" |grep -v ROOT | awk 'NR==1{printf"%s",$1}; NR>1{printf":%s",$1}'`
setenv LD_LIBRARY_PATH `echo $LD_LIBRARY_PATH | sed 's/:/\n/g' | grep -v "5.34.36" | grep -v ROOT | awk 'NR==1{printf"%s",$1}; NR>1{printf":%s",$1}'`

#setenv ROOTSYS /adaqfs/apps/ROOT/6.14-04
setenv ROOTSYS /adaqfs/apps/ROOT/6.16-00

setenv PATH "${ROOTSYS}/bin:${PATH}"
setenv LD_LIBRARY_PATH "${ROOTSYS}/lib:${LD_LIBRARY_PATH}"

setenv QW_DATA      /adaq1/data1/apar/
#setenv QW_ROOTFILES_OLD /aonl3/work1/apar_OLD/japanOutput
setenv QW_ROOTFILES /chafs2/work1/apar/japanOutput

alias root 'root -l'

