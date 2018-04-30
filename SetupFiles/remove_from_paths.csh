#!/bin/csh


if ($1 == "") then
    echo "\nUsage:  source remove_from_paths.csh <pattern to remove from paths>\n"
    echo "        This program will edit the PATH, SHLIB_PATH, and LD_LIBRARY_PATH"
    echo "        environment variables to remove paths containing a certain"
    echo "        pattern."
    echo "        This can be useful for switching between ROOT versions, or"
    echo "        QwAnalysis versions, where you wish to ensure that two library"
    echo "        versions are not both in the path."
    echo "\n        Example:  source remove_from_paths.csh "'$ROOTSYS'
    echo '        will remove the subdirectories of $ROOTSYS from the paths.'
else
    set pattern_to_remove = $1
    foreach pathvar (`printenv | awk -F"=" '{print $1}' | grep PATH`)
	setenv $pathvar `printenv $pathvar | sed 's/:/\n/g' | grep -v $pattern_to_remove | awk '{printf"%s:",$1}' | sed 's/:$//'`
    end
endif
