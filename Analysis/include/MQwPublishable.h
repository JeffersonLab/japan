#ifndef __MQWPUBLISHABLE__
#define __MQWPUBLISHABLE__

// System headers
#include <map>

// ROOT headers
#include "Rtypes.h"

// Qweak headers
#include "VQwHardwareChannel.h"

template<class U, class T>
class MQwPublishable_child {

  public:

  MQwPublishable_child() {fSelf = dynamic_cast<T*>(this); };
  MQwPublishable_child(const MQwPublishable_child& source) {fSelf = dynamic_cast<T*>(this);};

    virtual ~MQwPublishable_child() { };
    void SetParent(U* parent){fParent = parent;};
    U* GetParent() const {return fParent;};


 protected:
    /// \brief Retrieve the variable name from other subsystem arrays
    Bool_t RequestExternalValue(const TString& name, VQwHardwareChannel* value) const;
    /// \brief Retrieve the variable name from other subsystem arrays
    const VQwHardwareChannel* RequestExternalPointer(const TString& name) const;
    /// \brief Publish the value name with description from a subsystem in this array
    Bool_t PublishInternalValue(const TString name, const TString desc, const VQwHardwareChannel* element) const;


    /// The functions below should be specified in the fully derived classes.
 public:
    /// \brief Publish all variables of the subsystem
    virtual Bool_t PublishInternalValues() const = 0;
    /// \brief Try to publish an internal variable matching the submitted name
    virtual Bool_t PublishByRequest(TString device_name) = 0;

 private:
    U* fParent;
    T* fSelf;
};


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

  public:
    std::vector<std::vector<TString> > fPublishList;
    /// \brief Retrieve the variable name from other subsystem arrays
    Bool_t RequestExternalValue(const TString& name, VQwHardwareChannel* value) const;

    /// \brief Retrieve the variable name from other subsystem arrays
    const VQwHardwareChannel* RequestExternalPointer(const TString& name) const;
    
    /// \brief Retrieve the variable name from subsystems in this subsystem array
    virtual const VQwHardwareChannel* ReturnInternalValue(const TString& name) const;

    /// \brief Retrieve the variable name from subsystems in this subsystem array
    Bool_t ReturnInternalValue(const TString& name, VQwHardwareChannel* value) const;

    /// \brief List the published values and description in this subsystem array
    void ListPublishedValues() const;

    /// \brief Publish the value name with description from a subsystem in this array
    Bool_t PublishInternalValue(
        const TString name,
        const TString desc,
        const T* subsys,
        const VQwHardwareChannel* element);

  private:
    /// \brief Try to publish an internal variable matching the submitted name
    virtual Bool_t PublishByRequest(TString device_name);

    /// Published values
    std::map<TString, const VQwHardwareChannel*> fPublishedValuesDataElement;
    std::map<TString, const T*>                  fPublishedValuesSubsystem;
    std::map<TString, TString>                   fPublishedValuesDescription;
 
};

#endif
