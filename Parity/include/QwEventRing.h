/**********************************************************\
* File: QwEventRing.h                                      *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2009-11-08 15:40>                           *
\**********************************************************/

#ifndef __QwEventRing__
#define __QwEventRing__

#include <vector>

#include <fstream>
#include "QwSubsystemArrayParity.h"

class QwEventRing {

/******************************************************************
 *  Class: QwEventRing
 *  Description : The event buffer to reduce  beam trips effects on running 
 *  averages.
 *
 ******************************************************************/
 private:
  QwEventRing();

 public:
  QwEventRing(QwOptions &options, QwSubsystemArrayParity &event);
  QwEventRing(QwSubsystemArrayParity &event, Int_t ring_size); //this will create a fixed size event ring
  virtual ~QwEventRing() { };

  /// \brief Define options
  static void DefineOptions(QwOptions &options);
  /// \brief Process options
  void ProcessOptions(QwOptions &options);

  /// \brief Add the subsystem to the ring
  void push(QwSubsystemArrayParity &event);
  /// \brief Return the last subsystem in the ring
  QwSubsystemArrayParity& pop();

  /// \brief Return the read status of the ring
  Bool_t IsReady();

 private:

  Int_t fRING_SIZE;//this is the length of the ring


  Int_t fNextToBeFilled;//counts events in the ring
  Int_t fNextToBeRead;//keep track off when to read next from the ring.

  
  Bool_t bEVENT_READY; //If kTRUE, the good events are added to the event ring. After a beam trip this is set to kFALSE
  //after discarding LEAVE_COUNT no.of good event this is set to kTRUE

  Bool_t bRING_READY; //set to true after ring is filled with good events and time to process them. Set to kFALSE after processing 
  //all the events in the ring
  std::vector<QwSubsystemArrayParity> fEvent_Ring;
  //to track all the rolling averages for stability checks
  QwSubsystemArrayParity fRollingAvg;
  
  //for debugging purposes
  FILE *out_file;   
  static const Bool_t bDEBUG=kFALSE;//kTRUE;
  static const Bool_t bDEBUG_Write=kFALSE;

  //State of the stability check - ON/OFF
  Bool_t bStability;

};




#endif
