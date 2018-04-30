/**
 *  \file   QwObjectCounter.h
 *  \brief  Memory management class to count object instantiations
 *
 *  \author wdconinc
 *  \date   Nov 12, 2010
 */

#ifndef QWOBJECTCOUNTER_H_
#define QWOBJECTCOUNTER_H_


/**
 *  \class QwObjectCounter
 *  \ingroup QwAnalysis
 *
 *  \brief Memory management structure to count objects
 *
 *  This memory counter object can be publicly inherited from by Qweak classes.
 *  It applies the "curiously recurring template pattern", and I didn't make up
 *  that name.
 */
template<typename T>
class QwObjectCounter {

  public:

    /// Default constructor
    QwObjectCounter() {
      ++fObjectsCreated;
      ++fObjectsAlive;
    };
    /// Copy constructor
    QwObjectCounter(const QwObjectCounter&) {
      ++fObjectsCreated;
      ++fObjectsAlive;
    };
    /// Destructor
    virtual ~QwObjectCounter() {
      --fObjectsAlive;
    };


    /// Get number of objects ever created
    static size_t GetObjectsCreated() {
      return fObjectsCreated;
    };

    /// Get number of objects still alive
    static size_t GetObjectsAlive() {
      return fObjectsAlive;
    };

  private:

    /// Number of objects ever created
    static size_t fObjectsCreated;
    /// Number of objects still alive
    static size_t fObjectsAlive;

};

/// Initialize objects ever created counter
template<typename T>
size_t QwObjectCounter<T>::fObjectsCreated = 0;

/// Initialize objects still alive counter
template<typename T>
size_t QwObjectCounter<T>::fObjectsAlive = 0;

#endif /* QWOBJECTCOUNTER_H_ */
