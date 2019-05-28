#ifndef __MQWSTOREOBJECTS__
#define __MQWSTOREOBJECTS__

// System headers
#include <vector>

// Root headers
#include "TObject.h"

// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"

class MQwStoreObjects {

  protected:

    /// Default constructor
    MQwStoreObjects() { }
    /// Copy constructor
    MQwStoreObjects(const MQwStoreObjects& source) { }
    /// Virtual destructor
    virtual ~MQwStoreObjects() { }

    /// Arithmetic assignment operator:  Should only copy event-based data.
    /// In this particular class, there is no event-based data.
    virtual MQwStoreObjects& operator=(const MQwStoreObjects& value) {
      return *this;
    }

  private:

    /// \name List of dependent objects
    std::vector<std::pair<MQwStoreObjects*,std::string>> fMQwObjectsList;

  public:

    /// \name Objects construction and maintenance
    /// @{
    /// Register dependent subsystem
    virtual void RegisterDependentForObjects(MQwStoreObjects* value, std::string prefix) {
      fMQwObjectsList.push_back(std::make_pair(value,prefix));
    };
    virtual void ConstructAllObjects(TDirectory *folder, std::string prefix) {
      for (auto dep  = fMQwObjectsList.begin();
                dep != fMQwObjectsList.end();
                ++dep) {
        std::string fullprefix = (*dep).second + prefix;
        (*dep).first->ConstructAllObjects(folder, fullprefix);
      }
      ConstructObjects(folder, prefix);
    };
    /// \brief Construct the objects for this subsystem in a folder with a prefix
    virtual void ConstructObjects(TDirectory *folder = NULL, std::string prefix = "") { };
    /// \brief Construct the objects for this subsystem in a folder with a prefix
    virtual void ConstructAllObjects(QwRootFile *rootfile, std::string prefix = "") {
      for (auto dep  = fMQwObjectsList.begin();
                dep != fMQwObjectsList.end();
                ++dep) {
        (*dep).first->ConstructAllObjects(rootfile, prefix);
      }
      ConstructObjects(rootfile, prefix);
    };
    /// \brief Construct the objects for this subsystem in a folder with a prefix
    virtual void ConstructObjects(QwRootFile *rootfile, std::string prefix = "") {
      rootfile->ConstructObjects(prefix, *this);
    };
    /// @}


}; // class MQwStoreObjects

#endif // __MQWSTOREOBJECTS__

