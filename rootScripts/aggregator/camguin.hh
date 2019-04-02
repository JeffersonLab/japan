/********************************************************************
 *                                                                  *
 * Author:  Cameron Clarke                                          *
 * Date:    3-28-2019                                               *
 * Title:   Continuous Aggregate Monitor Graphical User INterfacea  *
 * Purpose: A set of methods useful for reading a root tree, doing  *
 *          some analysis, and printing those results into an       *
 *          updatable ROOT file.                                    *
 *          This is a fully functioning macro whose methods stand   *
 *          alone in PANGUIN and in ROOT in general as well, and it *
 *          represents several possible ways of accessing and       *
 *          manipulating ROOT output.                               *
 *                                                                  *
 *******************************************************************/
#include <iostream>
#include <string>
#include <stdlib.h>
#include <dirent.h>
#include "camIO.hh"
#include "camHist.hh"
#include "camAna.hh"
using namespace std;

