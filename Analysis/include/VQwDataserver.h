#ifndef __VQwDataserver_h__
#define __VQwDataserver_h__

#include <iostream>

#include "VQwSystem.h"
#include "QwHitContainer.h"

class VQwDataserver : public VQwSystem {

  private:
    VQwDataserver& operator= (const VQwDataserver &value) {
      return *this;
    };

  public:
    VQwDataserver (const char* name): VQwSystem (name) { };

    virtual void NextEvent() {
      std::cout << "Error: Not implemented!" << std::endl;
      return;
    };
    // TODO (wdc) I don't want this to depend on QwHitContainer...
    virtual QwHitContainer* GetHitList() {
      std::cout << "Error: Not implemented!" << std::endl;
      return 0;
    };

};

#endif // __VQwAnalyzer_h__
