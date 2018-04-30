#ifndef __QwControl_h__
#define __QwControl_h__

#include "TThread.h"

#include "VQwSystem.h"
#include "VQwAnalyzer.h"
#include "VQwDataserver.h"

void* QwRunThread (void*);

class QwControl : public VQwSystem {

  private:
    TThread* fRunThread;	// thread with analyzer

    VQwAnalyzer* fAnalyzer;	// analyzer
    VQwDataserver* fDataserver;	// dataserver

    bool fIsOnline;	// Are we running online (CODA stream) or offline (ASCII)
    bool fIsFinished;	// Are we at the end of the data stream yet

  public:
    QwControl (const char* name, bool = false): VQwSystem(name) { };
    virtual ~QwControl() { };

    void Run();
    void Start();
    void OnlineLoop();
    void OfflineLoop();

    // analyzer creation and starting
    VQwAnalyzer* CreateAnalyzer (const char* name);
    void SetAnalyzer (VQwAnalyzer* analyzer);

    // dataserver creation and starting
    VQwDataserver* CreateDataserver (const char* name);
    void SetDataserver (VQwDataserver* dataserver);


    void SetIsOnline (bool online) { fIsOnline = online; };
    bool IsOnline () { return fIsOnline; };
    void SetIsFinished (bool finished) { fIsFinished = finished; };
    bool IsFinished () { return fIsFinished; };

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwControl,1)
  #endif
};

#endif // __QwControl_h__
