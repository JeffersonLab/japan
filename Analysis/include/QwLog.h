/*!
 * \file   QwLog.h
 * \brief  A logfile class, based on an identical class in the Hermes analyzer
 *
 * \author Wouter Deconinck
 * \date   2009-11-25
 */

#ifndef QWLOG_HH
#define QWLOG_HH

// System headers
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using std::string;

// Qweak headers
#include "QwTypes.h"
#include "QwColor.h"

/*!
 * \note Because QwOptions depends on QwLog, and QwLog depends also on QwOptions,
 * we cannot include QwOptions in the QwLog header file here.  QwLog is treated
 * as more basic than QwOptions.
 */

// Forward declarations
class QwOptions;

/*! \def QwOut
 *  \brief Predefined log drain for explicit output
 */
#define QwOut      gQwLog(QwLog::kAlways,__PRETTY_FUNCTION__)

/*! \def QwError
 *  \brief Predefined log drain for errors
 */
#define QwError    gQwLog(QwLog::kError,__PRETTY_FUNCTION__)

/*! \def QwWarning
 *  \brief Predefined log drain for warnings
 */
#define QwWarning  gQwLog(QwLog::kWarning,__PRETTY_FUNCTION__)

/*! \def QwMessage
 *  \brief Predefined log drain for regular messages
 */
#define QwMessage  gQwLog(QwLog::kMessage,__PRETTY_FUNCTION__)

/*! \def QwVerbose
 *  \brief Predefined log drain for verbose messages
 */
#define QwVerbose  gQwLog(QwLog::kVerbose,__PRETTY_FUNCTION__)

/*! \def QwDebug
 *  \brief Predefined log drain for debugging output
 */
#define QwDebug    gQwLog(QwLog::kDebug,__PRETTY_FUNCTION__)


/**
 *  \class QwLog
 *  \ingroup QwAnalysis
 *  \brief A logfile class
 *
 * This class should not be used directly. Instead one can write text to the
 * screen or a log file via the predefined log drains QwError, QwWarning,
 * QwMessage, QwVerbose and QwDebug.  A special log drain QwOut will always
 * be printed and is reserved for output \b explicitly requested by the user.
 *
 * An example could be:
\verbatim
 QwMessage << "Hello World !!!" << QwLog::endl;
\endverbatim
 */
class QwLog : public std::ostream {

  public:

    /// \brief Define available class options for QwOptions
    static void DefineOptions(QwOptions* options);
    /// \brief Process class options for QwOptions
    void ProcessOptions(QwOptions* options);
    // Note: this uses pointers as opposed to references, because as indicated
    // above the QwLog class cannot depend on the QwOptions class.  When using a
    // pointer we only need a forward declaration and we do not need to include
    // the header file QwOptions.h.

    //! Loglevels
    /*! enum of possible log levels */
    enum QwLogLevel {
      kAlways    = -1, /*!< Explicit output  */
      kError     =  0, /*!< Error loglevel   */
      kWarning   =  1, /*!< Warning loglevel */
      kMessage   =  2, /*!< Message loglevel */
      kVerbose   =  3, /*!< Verbose loglevel */
      kDebug     =  4  /*!< Debug loglevel   */
    };

    //! Log file open modes
    static const std::ios_base::openmode kTruncate;
    static const std::ios_base::openmode kAppend;

    /*! \brief The constructor
     */
    QwLog();

    /*! \brief The destructor
     */
    virtual ~QwLog();

    /*! \brief Determine whether the function name matches a specified list of regular expressions
     */
    bool                        IsDebugFunction(const string func_name);

    /*! \brief Initialize the log file with name 'name'
     */
    void                        InitLogFile(const std::string name, const std::ios_base::openmode mode = kAppend);

    /*! \brief Set the screen color mode
     */
    void                        SetScreenColor(bool flag);

    /*! \brief Set the screen log level
     */
    void                        SetScreenThreshold(int thr);

    /*! \brief Set the file log level
     */
    void                        SetFileThreshold(int thr);

    /*! \brief Set the stream log level
     */
    QwLog&                      operator()(const QwLogLevel level,
                                           const std::string func_sig  = "<unknown>");

    /*! \brief Stream an object to the output stream
     */
    template <class T> QwLog&   operator<<(const T &t) {
      if (fScreen && fLogLevel <= fScreenThreshold) {
        *(fScreen) << t;
      }
      if (fFile && fLogLevel <= fFileThreshold) {
        *(fFile) << t;
      }
      return *this;
    }

    /*! \brief Pass the ios_base manipulators
     */
#if (__GNUC__ >= 3)
    QwLog&                      operator<<(std::ios_base & (*manip) (std::ios_base &));
#endif
    QwLog&                      operator<<(std::ostream & (*manip) (std::ostream &));

    /*! \brief End of the line
     */
    static std::ostream&        endl(std::ostream&);

    /*! \brief Flush the streams
     */
    static std::ostream&        flush(std::ostream&);

  private:

    /*! \brief Get the local time
     */
    const char*                 GetTime();
    char                        fTimeString[128];

    //! Screen thresholds and stream
    QwLogLevel    fScreenThreshold;
    std::ostream *fScreen;
    //! File thresholds and stream
    QwLogLevel    fFileThreshold;
    std::ostream *fFile;
    //! Log level of this stream
    QwLogLevel fLogLevel;

    //! Flag to print function signature on warning or error
    bool fPrintFunctionSignature;

    //! List of regular expressions for functions that will have increased log level
    std::map<std::string,bool> fIsDebugFunction;
    std::vector<std::string> fDebugFunctionRegexString;

    //! Flag to disable color
    bool fUseColor;

    //! Flags only relevant for current line, but static for use in static function
    static bool fFileAtNewLine;
    static bool fScreenInColor;
    static bool fScreenAtNewLine;

};

extern QwLog gQwLog;

#endif
