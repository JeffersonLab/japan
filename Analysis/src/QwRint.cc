#include "QwRint.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TROOT.h>

// Global pointers
QwRint* gQwRint = NULL;

// Pointer to self
QwRint* QwRint::fExists = NULL;


//--------------------------------------------------------------------------
QwRint::QwRint (const char* appClassName, int* argc, char** argv,
		      void* options, int numOptions, bool noLogo)
: TRint (appClassName, argc, argv, options, numOptions, noLogo)
{
  gQwRint = this;

  // Qw-Root command prompt
  SetPrompt("Qw-Root [%d] ");

  // Pointer to self
  fExists = this;
}

//---------------------------------------------------------------------------
QwRint::~QwRint()
{
  // Reset point to self
  if (fExists == this)
    fExists = NULL;
}
