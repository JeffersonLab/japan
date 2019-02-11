/**********************************************************\
* File: QwParameterFile.h                                  *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWPARAMETERFILE__
#define __QWPARAMETERFILE__


// System headers
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <set>

// ROOT headers
#include "Rtypes.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMacro.h"
#include "TList.h"

// Boost headers
#include "boost/lexical_cast.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/filesystem/path.hpp"
namespace bfs = boost::filesystem;
using boost::lexical_cast;

// Qweak headers
#include "QwLog.h"

///
/// \ingroup QwAnalysis
class QwParameterFile {

  public:

    static UInt_t GetUInt(const TString &varvalue);

  public:

    QwParameterFile(const std::string& filename);
    QwParameterFile(const std::stringstream& stream);
    virtual ~QwParameterFile() { };

    /// Access the streambuf pointer in the same way as on a std::ifstream
    std::streambuf* rdbuf() const {return fStream.rdbuf(); };

    /// Add a directory to the search path
    static void AppendToSearchPath(const TString& searchdir);

    /// Set the current run number for looking up the appropriate parameter file
    static void SetCurrentRunNumber(const UInt_t runnumber) { fCurrentRunNumber = runnumber; };

    /// Set various sets of special characters
    void SetCommentChars(const std::string value)    { fCommentChars = value; };
    void SetWhitespaceChars(const std::string value) { fWhitespaceChars = value; };
    void SetTokenSepChars(const std::string value)   { fTokenSepChars = value; };
    void SetSectionChars(const std::string value)    { fSectionChars = value; };
    void SetModuleChars(const std::string value)     { fModuleChars = value; };

    Bool_t ReadNextLine() {
      fCurrentPos = 0;
      std::string tmp;
      return ReadNextLine(tmp);
    };
    Bool_t ReadNextLine(std::string &varvalue) {
      Bool_t status = kFALSE;
      if (fBeGreedy) status = ReadNextLine_Greedy(varvalue);
      else           status = ReadNextLine_Single(varvalue);
      return status;
    }
    Bool_t ReadNextLine_Greedy(std::string &varvalue);  
    Bool_t ReadNextLine_Single(std::string &varvalue) {
      fCurrentPos = 0;
      if (! getline(fStream, fLine))
        // No next line
        return 0;
      else {
        // Copy next line
        varvalue = fLine;
        // Allow 'append'
        std::string tmpname, tmpvalue;
        if (HasVariablePair(" ",tmpname,tmpvalue)) {
          if (tmpname == "append") {
            // Test for recursion in file nesting
            static int nested_depth = 0;
            if (nested_depth++ > 5) {
              std::cout << "Parameter file recursion not allowed!" << std::endl;
              return 0;
            }
            // Stream nested file into this file
            QwParameterFile nested_file(tmpvalue.c_str());
            fStream << nested_file.rdbuf();
            // Read line from appended file
            return ReadNextLine(varvalue);
          }
        }
        return 1;
      }
    };

    void TrimWhitespace(TString::EStripType head_tail = TString::kBoth);
    void TrimComment(const char commentchar);
    void TrimComment(const std::string& commentchars);
    void TrimComment() { TrimComment(fCommentChars); };
    void TrimSectionHeader();
    void TrimModuleHeader();

    TString LastString(TString in, char* delim);
    TString GetParameterFileContents();

    Bool_t LineIsEmpty(){return fLine.empty();};
    Bool_t IsEOF(){ return fStream.eof();};

    /// \brief Get next token as a string
    std::string GetNextToken(const std::string& separatorchars);
    std::string GetNextToken() { return GetNextToken(fTokenSepChars); };

    /// Get next token into specific type
    template <typename T>
    T GetTypedNextToken() {
      return ConvertValue<T>(GetNextToken());
    }

    std::string GetLine() { return fLine; };
    void AddLine(const std::string& line) { fStream << line << std::endl; };

    void RewindToFileStart() { fStream.clear(); fStream.seekg(0, std::ios::beg); };
    void RewindToLineStart() { fCurrentPos = 0; };

    Bool_t HasValue(TString& vname);

    Bool_t HasVariablePair(const std::string& separatorchars, std::string& varname, std::string& varvalue);
    Bool_t HasVariablePair(const std::string& separatorchars, TString& varname, TString& varvalue);

    Bool_t FileHasVariablePair(const std::string& separatorchars, const std::string& varname, std::string& varvalue);
    Bool_t FileHasVariablePair(const std::string& separatorchars, const TString& varname, TString& varvalue);

    Bool_t LineHasSectionHeader();
    Bool_t LineHasSectionHeader(std::string& secname);
    Bool_t LineHasSectionHeader(TString& secname);

    Bool_t LineHasModuleHeader();
    Bool_t LineHasModuleHeader(std::string& secname);
    Bool_t LineHasModuleHeader(TString& secname);

    Bool_t FileHasSectionHeader(const std::string& secname);
    Bool_t FileHasSectionHeader(const TString& secname);

    Bool_t FileHasModuleHeader(const std::string& secname);
    Bool_t FileHasModuleHeader(const TString& secname);

    ///  \brief Skips from the beginning of the section
    ///  'secname' until the first section that does not
    ///  have that name.
    Bool_t SkipSection(std::string secname);

    /// \brief Rewinds to the start and read until it finds next section header
    QwParameterFile* ReadSectionPreamble();
    QwParameterFile* ReadUntilNextSection(const bool add_current_line = false);
    QwParameterFile* ReadNextSection(std::string &secname, const bool keep_header = false);
    QwParameterFile* ReadNextSection(TString &secname, const bool keep_header = false);
    QwParameterFile* ReadNextSection(const bool keep_header = false) {
      std::string dummy;
      return ReadNextSection(dummy, keep_header);
    };

    /// \brief Rewinds to the start and read until it finds next module header
    QwParameterFile* ReadModulePreamble();
    QwParameterFile* ReadUntilNextModule(const bool add_current_line = false);
    QwParameterFile* ReadNextModule(std::string &secname, bool keep_header = false);
    QwParameterFile* ReadNextModule(TString &secname, bool keep_header = false);
    QwParameterFile* ReadNextModule(const bool keep_header = false) {
      std::string dummy;
      return ReadNextModule(dummy, keep_header);
    };

    friend std::ostream& operator<< (std::ostream& stream, const QwParameterFile& file);


    const TString GetParamFilename() {return fBestParamFileName;};
    const TString GetParamFilenameAndPath() {return fBestParamFileNameAndPath;};

    const std::pair<TString, TString> GetParamFileNameContents() {
      return std::pair<TString, TString>(GetParamFilename(), GetParameterFileContents());
    };

    void SetParamFilename();

    void EnableGreediness() {fBeGreedy=kTRUE;};
    void DisableGreediness(){fBeGreedy=kFALSE;};

    void AddBreakpointKeyword(std::string keyname);

    void Close() { fFile.close();};

    Bool_t HasNewPairs(){
      Bool_t status = fHasNewPairs;
      if (fHasNewPairs) fHasNewPairs=kFALSE;
      return status;
    };

    template <typename T> 
      Bool_t ReturnValue(const std::string keyname, T &retvalue){
      std::string value;
      Bool_t status = GetKeyValue(keyname, value);
      if (status){
	retvalue = ConvertValue<T>(value);
      }
      return status;
    }
    template <typename T> 
      Bool_t PopValue(const std::string keyname, T &retvalue){
      std::string value;
      Bool_t status = GetKeyValue(keyname, value, kTRUE);
      if (status){
	retvalue = ConvertValue<T>(value);
      }
      return status;
    };

    

  protected:
    void Trim(const std::string& chars, std::string& token, TString::EStripType head_tail = TString::kBoth);
    void TrimWhitespace(std::string &token, TString::EStripType head_tail);

  public:
    /// Convert string value into specific type
    template <typename T>
    T ConvertValue(const std::string& value) {
      T retvalue;
      if (value.size() == 0) {
        retvalue = 0; // and pray
      } else {
        std::istringstream stream1;
        stream1.str(value);
        stream1 >> retvalue;
      }
      return retvalue;
    }

  private:

    /// Find the first file in a directory that conforms to the run label
    int FindFile(const bfs::path& dir_path,    // in this directory,
                 const std::string& file_stem, // search for this stem,
                 const std::string& file_ext,  // search for this extension,
                 bfs::path& path_found);       // placing path here if found

    /// Open a file
    bool OpenFile(const bfs::path& path_found);
  //  TString fCurrentSecName;     // Stores the name of the current section  read
  //  TString fCurrentModuleName;  // Stores the name of the current module  read
    TString fBestParamFileName;
    TString fBestParamFileNameAndPath;

  public:

    /// \brief Parse a range of integers as #:# where either can be missing
    static std::pair<int,int> ParseIntRange(const std::string& separatorchars, const std::string& range);


  protected:

    // List of search paths
    static std::vector<bfs::path> fSearchPaths;

    // Current run number
    static UInt_t fCurrentRunNumber;

    // Default comment, whitespace, section, module characters
    static const std::string kDefaultCommentChars;
    static const std::string kDefaultWhitespaceChars;
    static const std::string kDefaultTokenSepChars;
    static const std::string kDefaultSectionChars;
    static const std::string kDefaultModuleChars;

    // Local comment, whitespace, section, module characters
    std::string fCommentChars;
    std::string fWhitespaceChars;
    std::string fTokenSepChars;
    std::string fSectionChars;
    std::string fModuleChars;


    // File and stream
    const std::string fFilename;
    std::ifstream fFile;
    std::stringstream fStream;

    // Current line and position
    std::string fLine;      /// Internal line storage
    size_t fCurrentPos;     /// Current position in the line

 protected:

    Bool_t GetKeyValue(const std::string keyname, std::string &retvalue,
		       Bool_t should_erase = kFALSE){
      Bool_t status = kFALSE;
      std::map<std::string,std::string>::iterator it;
      it = fKeyValuePair.find(keyname);
      if (it != fKeyValuePair.end()) {
	status = kTRUE;
	retvalue = (*it).second;
	if (should_erase){
	  fKeyValuePair.erase(it);
	}
      }
      return status;
    }


    Bool_t fBeGreedy;
    std::set<std::string> fBreakpointWords;
    std::map<std::string, std::string> fKeyValuePair;
    Bool_t fHasNewPairs;

  private:

    // Private constructor
    QwParameterFile()
    : fCommentChars(kDefaultCommentChars),
      fWhitespaceChars(kDefaultWhitespaceChars),
      fTokenSepChars(kDefaultTokenSepChars),
      fSectionChars(kDefaultSectionChars),
      fModuleChars(kDefaultModuleChars),
      fFilename("empty"),
      fCurrentPos(0),
      fBeGreedy(kFALSE),
      fHasNewPairs(kFALSE)
    { };

    // Private copy constructor
    QwParameterFile(const QwParameterFile& input)
    : fCommentChars(input.fCommentChars),
      fWhitespaceChars(input.fWhitespaceChars),
      fTokenSepChars(input.fTokenSepChars),
      fSectionChars(input.fSectionChars),
      fModuleChars(input.fModuleChars),
      fFilename(input.fFilename),
      fCurrentPos(input.fCurrentPos),
      fBeGreedy(input.fBeGreedy),
      fHasNewPairs(input.fHasNewPairs)
    { };

    // Set to end of file
    void SetEOF() { fStream.setstate(std::ios::eofbit); };

}; // class QwParameterFile


template <>
inline std::string QwParameterFile::GetTypedNextToken<std::string>(){
  return GetNextToken();
}

template <>
inline UInt_t QwParameterFile::ConvertValue<UInt_t>(const std::string& value) {
  return GetUInt(value);
}

template <>
inline std::string QwParameterFile::ConvertValue<std::string>(const std::string& value) {
  return value;
}

template <>
inline TString QwParameterFile::ConvertValue<TString>(const std::string& value) {
  return TString(value.c_str());
}

#endif // __QWPARAMETERFILE__
