#include "MQwPublishable.h"

#include "QwSubsystemArray.h"
#include "VQwSubsystem.h"

/**
 * Retrieve the variable name from other subsystem arrays
 * @param name Variable name to be retrieved
 * @param value (return) Data element with the variable name
 * @return True if the variable is found, false if not found
 */
template<class U, class T>
Bool_t MQwPublishable<U,T>::RequestExternalValue(const TString& name, VQwHardwareChannel* value) const
{
  //  If this has a parent, we should escalate the call to that object,
  //  but so far we don't have that capability.
  return ReturnInternalValue(name, value);
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @return Data element with the variable name, null if not found
 */
template<class U, class T>
const VQwHardwareChannel* MQwPublishable<U,T>::ReturnInternalValue(const TString& name) const
{
  //  First try to find the value in the list of published values.
  std::map<TString, const VQwHardwareChannel*>::const_iterator iter1 =
      fPublishedValuesDataElement.find(name);
  if (iter1 != fPublishedValuesDataElement.end()) {
    return iter1->second;
  }
  //  Second, ask the subsystem that has claimed the value
  typename std::map<TString, const T*>::const_iterator iter2 =
      fPublishedValuesSubsystem.find(name);
  if (iter2 != fPublishedValuesSubsystem.end()) {
    return (iter2->second)->ReturnInternalValue(name);
  }
  //  If the value is not yet published, try requesting it.
  if (const_cast<MQwPublishable*>(this)->PublishByRequest(name)){
    iter1 = fPublishedValuesDataElement.find(name);
    if (iter1 != fPublishedValuesDataElement.end()) {
      return iter1->second;
    }
    QwError << "PublishByRequest succeeded, but can't find the record for "
            << name << QwLog::endl;

  } else {
    QwError << "PublishByRequest failed for " << name << QwLog::endl;
  }
  //  Not found
  return 0;
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @param value (return) Data element with the variable name
 * @return True if the variable was found, false if not found
 */
template<class U, class T>
Bool_t MQwPublishable<U,T>::ReturnInternalValue(const TString& name, VQwHardwareChannel* value) const
{
  Bool_t foundit = kFALSE;

  // Check for null pointer
  if (! value)
    QwWarning << "MQwPublishable::ReturnInternalValue requires that "
              << "'value' be a non-null pointer to a VQwHardwareChannel."
              << QwLog::endl;

  //  Get a const pointer to the internal value
  VQwHardwareChannel* internal_value = const_cast<VQwHardwareChannel*>(ReturnInternalValue(name));
  if (value && internal_value && typeid(value) == typeid(internal_value)) {
    value->AssignValueFrom(internal_value);
    foundit = kTRUE;
  } else
    QwWarning << "MQwPublishable::ReturnInternalValue: name \""
              << name << "\" not found in array." << QwLog::endl;

  return foundit;
}

/**
 * Publish the value name with description from a subsystem in this array
 * @param name Name of the variable
 * @param desc Description of the variable
 * @param subsys Subsystem that contains the variable
 * @param element Data element that contains the variable
 * @return True if the variable could be published, false if not published
 */
template<class U, class T>
Bool_t MQwPublishable<U,T>::PublishInternalValue(
    const TString name,
    const TString desc,
    const T* subsys,
    const VQwHardwareChannel* element)
{
  if (fPublishedValuesSubsystem.count(name) > 0) {
    QwError << "Attempting to publish existing variable key!" << QwLog::endl;
    ListPublishedValues();
    return kFALSE;
  }
  fPublishedValuesSubsystem[name] = subsys;
  fPublishedValuesDescription[name] = desc;
  fPublishedValuesDataElement[name] = element;
  return kTRUE;
}

/**
 * Try to publish an internal variable matching the submitted name
 * @param device_name Name of the desired published variable
 * @return True if the variable could be published, false if not published
 */
template<class U, class T>
Bool_t MQwPublishable<U,T>::PublishByRequest(TString device_name)
{
  Bool_t status = kFALSE;
  if (fPublishedValuesSubsystem.count(device_name) > 0) {
    QwError << "MQwPublishable::PublishByRequest:  Channel "
            << device_name << " has already been published."
            << QwLog::endl;
    ListPublishedValues();
    status = kTRUE;
  } else {
    U* u = dynamic_cast<U*>(this);
    if (not u->empty()) {
      for (auto subsys = u->begin(); subsys != u->end(); ++subsys)
      {
        status = (*subsys)->PublishByRequest(device_name);
        if (status) break;
      }
      // Report failure to publish
      if (! status) {
        QwError << "MQwPublishable::PublishByRequest: Failed to publish channel name: "
            << device_name << QwLog::endl;
      }
    }
  }
  return status;
}


/**
 * List the published values and description in this subsystem array
 */
template<class U, class T>
void MQwPublishable<U,T>::ListPublishedValues() const
{
  QwOut << "List of published values:" << QwLog::endl;
  std::map<TString,TString>::const_iterator iter;
  for (iter  = fPublishedValuesDescription.begin();
       iter != fPublishedValuesDescription.end(); iter++) {
    QwOut << iter->first << ": " << iter->second << QwLog::endl;
  }
}

/**
 * Retrieve the variable name from subsystems in this subsystem array
 * @param name Variable name to be retrieved
 * @return Data element with the variable name
 */
template<class U, class T>
VQwHardwareChannel* MQwPublishable<U,T>::ReturnInternalValueForFriends(const TString& name) const
{
  return const_cast<VQwHardwareChannel*>(ReturnInternalValue(name));
}

// Force instantiation of template classes we will need
template class MQwPublishable<QwSubsystemArray,VQwSubsystem>;
