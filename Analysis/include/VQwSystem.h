#ifndef __VQwSystem_h__
#define __VQwSystem_h__

#include "TROOT.h"
#include "TNamed.h"

class VQwSystem : public TNamed {

  public:
    VQwSystem (const char* name): TNamed (name, "Qweak-ROOT") { };
    virtual ~VQwSystem() { };

};

#endif // __VQwSystem_h__
