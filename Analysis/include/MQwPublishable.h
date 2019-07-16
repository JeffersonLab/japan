#ifndef __MQWPUBLISHABLE__
#define __MQWPUBLISHABLE__

// System headers
#include <map>

// ROOT headers
#include "Rtypes.h"

// Qweak headers
#include "VQwHardwareChannel.h"

template<class U, class T>
class MQwPublishable {

  public:

    MQwPublishable() { };
    MQwPublishable(const MQwPublishable& source) {
      fPublishedValuesDataElement.clear();
      fPublishedValuesSubsystem.clear();
      fPublishedValuesDescription.clear();
    }

    virtual ~MQwPublishable() { };

    /// Friend with data handler classes
    friend class QwCombiner;
    friend class QwCorrelator;
    friend class VQwDataHandler;

  public:
    std::vector<std::vector<TString> > fPublishList;
    /// \brief Retrieve the variable name from other subsystem arrays
    Bool_t RequestExternalValue(const TString& name, VQwHardwareChannel* value) const;

    /// \brief Retrieve the variable name from subsystems in this subsystem array
    const VQwHardwareChannel* ReturnInternalValue(const TString& name) const;

    /// \brief Retrieve the variable name from subsystems in this subsystem array
    Bool_t ReturnInternalValue(const TString& name, VQwHardwareChannel* value) const;

    /// \brief Publish the value name with description from a subsystem in this array
    Bool_t PublishInternalValue(
        const TString name,
        const TString desc,
        const T* subsys,
        const VQwHardwareChannel* element);

    /// \brief List the published values and description in this subsystem array
    void ListPublishedValues() const;

  private:

    /// \brief Try to publish an internal variable matching the submitted name
    Bool_t PublishByRequest(TString device_name);

    /// \brief Retrieve the variable name from subsystems in this subsystem array
    VQwHardwareChannel* ReturnInternalValueForFriends(const TString& name) const;

    /// Published values
    std::map<TString, const VQwHardwareChannel*> fPublishedValuesDataElement;
    std::map<TString, const T*>                  fPublishedValuesSubsystem;
    std::map<TString, TString>                   fPublishedValuesDescription;
 
};

#endif
