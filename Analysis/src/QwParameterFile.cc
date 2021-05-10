///  @file QwParameterFile.cc

#include "QwParameterFile.h"

// System headers
#include <sstream>
#include <climits>
#include <algorithm>
#include <cctype>

#ifndef BOOST_VERSION
#include "boost/version.hpp"
#endif

// Qweak headers
#include "QwLog.h"

// Initialize the list of search paths
std::vector<bfs::path> QwParameterFile::fSearchPaths;

// Set current run number to zero
UInt_t QwParameterFile::fCurrentRunNumber = 0;

// Set default comment, whitespace, section, module characters
const std::string QwParameterFile::kDefaultCommentChars = "#!;";
const std::string QwParameterFile::kDefaultWhitespaceChars = " \t\r";
const std::string QwParameterFile::kDefaultTokenSepChars = ", \t";
const std::string QwParameterFile::kDefaultSectionChars = "[]";
const std::string QwParameterFile::kDefaultModuleChars = "<>";

/**
 * Append a directory to the list of search paths
 * @param searchdir Directory to be added
 */
void QwParameterFile::AppendToSearchPath(const TString& searchdir)
{
  bfs::path tmppath(searchdir.Data());
  if( bfs::exists(tmppath) && bfs::is_directory(tmppath)) {
    std::cout << tmppath.string()
	      << " is a directory; adding it to the search path\n";
    fSearchPaths.push_back(tmppath);
  } else if( bfs::exists(tmppath)) {
    std::cout<<tmppath.string()<<" exists but is not a directory.\n";
  } else {
    std::cout<<tmppath.string()<<" doesn't exist.\n";
  }
}

/**
 * Convert a string number value to an unsigned integer
 * @param varvalue String with decimal or hexadecimal number
 * @return Unsigned integer
 */
UInt_t QwParameterFile::GetUInt(const TString& varvalue)
{
  UInt_t value = 0;
  if (varvalue.IsDigit()){
    value = varvalue.Atoi();
  } else if (varvalue.BeginsWith("0x") || varvalue.BeginsWith("0X")
	     || varvalue.BeginsWith("x") || varvalue.BeginsWith("X")
	     || varvalue.IsHex()){
    //any whitespace ?
    Int_t end = varvalue.Index(" ");
    std::istringstream stream1;
    if (end == -1){
      stream1.str(varvalue.Data());
    } else {
      //make temporary string, removing whitespace
      Int_t start = 0;
      TString tmp;
      //loop over all whitespace
      while (end > -1) {
        tmp += varvalue(start, end-start);
        start = end+1;
        end = varvalue.Index(" ", start);
      }
      //finally add part from last whitespace to end of string
      end = varvalue.Length();
      tmp += varvalue(start, end-start);
      stream1.str(tmp.Data());
    }
    stream1 >> std::hex >> value;
  }
  return value;
}


/**
 * Constructor
 * @param stream String stream for reading
 */
QwParameterFile::QwParameterFile(const std::stringstream& stream)
: fCommentChars(kDefaultCommentChars),
  fWhitespaceChars(kDefaultWhitespaceChars),
  fTokenSepChars(kDefaultTokenSepChars),
  fSectionChars(kDefaultSectionChars),
  fModuleChars(kDefaultModuleChars),
  fFilename("stream"),
  fBeGreedy(kFALSE)
{
  fStream << stream.rdbuf();
}


/**
 * Constructor
 * @param name Name of the file to be opened
 *
 * If file starts with an explicit slash ('/'), it is assumed to be a full path.
 */
QwParameterFile::QwParameterFile(const std::string& name)
: fCommentChars(kDefaultCommentChars),
  fWhitespaceChars(kDefaultWhitespaceChars),
  fTokenSepChars(kDefaultTokenSepChars),
  fSectionChars(kDefaultSectionChars),
  fModuleChars(kDefaultModuleChars),
  fFilename(name),
  fBeGreedy(kFALSE)
{
  // Create a file from the name
  bfs::path file(name);

  // Immediately try to open absolute paths and return
  if (name.find("/") == 0) {
    if (OpenFile(file) == false) {
      QwWarning << "Could not open absolute path " << name << ". "
                << "Parameter file will remain empty." << QwLog::endl;
      SetEOF();
      return;
    }

    // Print file name of file that was used
    QwMessage << "Parameter file: "
              << QwColor(Qw::kGreen)  << file.string()
              << QwColor(Qw::kNormal) << QwLog::endl;

    // Else, loop through search path and files
  } else {

#if BOOST_VERSION >= 104600
    // Separate file in stem and extension
    std::string file_stem = file.stem().string();
    std::string file_ext = file.extension().string();
#elif BOOST_VERSION >= 103600
    // Separate file in stem and extension
    std::string file_stem = file.stem();
    std::string file_ext = file.extension();
#else
    // Separate file in stem and extension
    std::string file_stem = bfs::basename(file);
    std::string file_ext = bfs::extension(file);
#endif

    // Find the best match
    Int_t best_score = 0;
    bfs::path best_path;
    for (size_t i = 0; i < fSearchPaths.size(); i++) {

      bfs::path path;
      Int_t score = FindFile(fSearchPaths[i], file_stem, file_ext, path);
      if (score > best_score) {
        // Found file with better score
        best_score = score;
        best_path  = path;
      } else if (score == best_score) {
        // Found file with identical score
        QwWarning << "Equally likely parameter files encountered: " << best_path.string()
                  << " and " << path.string() << QwLog::endl;
        QwMessage << "Analysis will use parameter file: " << best_path.string()
                  << QwLog::endl;
      }
    } // end of loop over search paths

    // File not found
    if (best_score == 0) {
      QwError << "Could not find any parameter file with name " << name << ". "
              << "Parameter file will remain empty." << QwLog::endl;
      SetEOF();
      exit(-1);
      return;
    }

    // Found but unable to open
    if (best_score > 0 && OpenFile(best_path) == false) {
      QwError << "Could not open parameter file " << best_path.string() << ". "
              << "Parameter file will remain empty." << QwLog::endl;
      SetEOF();
      exit(-1);
      return;
    }

    // Print file name of file that was used
    QwMessage << "Parameter file: "
              << QwColor(Qw::kGreen)  << best_path.string()
              << QwColor(Qw::kNormal) << QwLog::endl;
  }
}


/**
 * Open a file at the specified location
 * @param file Path to file to be opened
 * @return False if the file could not be opened
 */
bool QwParameterFile::OpenFile(const bfs::path& file)
{
  Bool_t local_debug = false;

  Bool_t status = false;

  Bool_t check_whether_path_exists_and_is_a_regular_file = false;
  
  // Check whether path exists and is a regular file
#if BOOST_VERSION >= 103600
  check_whether_path_exists_and_is_a_regular_file = bfs::exists(file) && bfs::is_regular_file(file);
#elif BOOST_VERSION >= 103400
  check_whether_path_exists_and_is_a_regular_file = bfs::exists(file) && bfs::is_regular(file);
#else
  check_whether_path_exists_and_is_a_regular_file = bfs::exists(file); /* pray */
#endif

  if (check_whether_path_exists_and_is_a_regular_file) {
    
    fBestParamFileNameAndPath = file.string();
    this->SetParamFilename();
    
    // Connect stream (fFile) to file
    fFile.open(file.string().c_str());
    if (! fFile.good())
      QwError << "QwParameterFile::OpenFile Unable to read parameter file "
	      << file.string() << QwLog::endl;
    // Load into stream
    fStream << fFile.rdbuf();
    status = true;
    if(local_debug) {
      std::cout << "------before close------------" << std::endl;
      std::cout << fStream.str() << std::endl;
    }

    // fFile.clear();
    // fFile.close(); // disconnet file
    //    this->Test();
  } else {

    // File does not exist or is not a regular file
    QwError << "QwParameterFile::OpenFile Unable to open parameter file "
	    << file.string() << QwLog::endl;
    status = false;
  }
  if(local_debug) {
    std::cout << "-------after close ----------" << std::endl;
    std::cout << fStream.str() << std::endl;
  }
  
  return status;
}


/**
 * Find the file in a directory with highest-scoring run label
 * @param directory Directory to search in
 * @param file_stem File name stem to search for
 * @param file_ext File name extensions to search for
 * @param best_path (returns) Path to the highest-scoring file
 * @return Score of file
 */
int QwParameterFile::FindFile(
	const bfs::path&   directory,
	const std::string& file_stem,
	const std::string& file_ext,
	bfs::path&         best_path)
{
  // Return false if the directory does not exist
  if (! bfs::exists(directory)) return false;

  // Default score indicates no match found
  int best_score = -1;
  int score = -1;
  // Multiple overlapping open-ended ranges
  int open_ended_latest_start = 0;
  int open_ended_range_score = 0;

  // Loop over all files in the directory
  // note: default iterator constructor yields past-the-end
  bfs::directory_iterator end_iterator;
  for (bfs::directory_iterator file_iterator(directory);
       file_iterator != end_iterator;
       file_iterator++) {

    // Match the stem and extension
    // note: filename() returns only the file name, not the path
#if BOOST_VERSION >= 104600
    std::string file_name = file_iterator->path().filename().string();
#elif BOOST_VERSION >= 103600
    std::string file_name = file_iterator->filename();
#else
    std::string file_name = file_iterator->leaf();
#endif
    // stem
    size_t pos_stem = file_name.find(file_stem);
    if (pos_stem != 0) continue;
    // extension (reverse find)
    size_t pos_ext = file_name.rfind(file_ext);
    if (pos_ext != file_name.length() - file_ext.length()) continue;

    // Scores (from low to high)
    const int score_no_run_label = 1;
    // Open run range:
    // score increments each time a higher-starting range is found
    // difference between min and max gives maximum number of open ranges
    const int score_open_ended_run_range_min = 1000;
    const int score_open_ended_run_range_max = 9000;
    // Close run range:
    // score decremented from max by size of run range, small range -> high score
    // difference between min and max gives maximum number of runs in closed range
    const int score_closed_run_range_min = 10000;
    const int score_closed_run_range_max = 90000;
    // Single run label will always have maximum score
    const int score_single_run_label = INT_MAX;

    // Determine run label length
    size_t label_length = pos_ext - file_stem.length();
    // no run label
    if (label_length == 0) {
      score = score_no_run_label;
    } else {
      // run label starts after dot ('.') and that dot is included in the label length
      if (file_name.at(pos_stem + file_stem.length()) == '.') {
        std::string label = file_name.substr(pos_stem + file_stem.length() + 1, label_length - 1);
        std::pair<int,int> range = ParseIntRange("-",label);
        int run = fCurrentRunNumber;
        if ((range.first <= run) && (run <= range.second)) {

          // run is in single-value range
          if (range.first == range.second) {
            score = score_single_run_label;

          // run is in double-value range
          } else if (range.second < INT_MAX) {
            int number_of_runs = abs(range.second - range.first);
            score = score_closed_run_range_max - number_of_runs;
            if (score < score_closed_run_range_min) {
              score = score_closed_run_range_min;
              QwError << "Too many runs in closed run range for " << file_stem << QwLog::endl;
              QwWarning << "Range is from " << range.first << " to " << range.second << QwLog::endl;
            }

          // run is in open-ended range
          } else if (range.second == INT_MAX) {
            // each matching open-ended range
            if (range.first > open_ended_latest_start) {
              open_ended_latest_start = range.first;
              open_ended_range_score++;
              score = score_open_ended_run_range_min + open_ended_range_score;
              if (score > score_open_ended_run_range_max) {
                score = score_open_ended_run_range_max;
                QwError << "Too many open ended run ranges for " << file_stem << QwLog::endl;
              }

            } else score = score_open_ended_run_range_min;
          }
        } else
          // run not in range
          score = -1;
      } else
	// run label does not start with a dot (i.e. partial match of stem)
	score = -1;
    }

    // Look for the match with highest score
    if (score > best_score) {
      best_path = *file_iterator;
      best_score = score;
    }
  }
  return best_score;
}


void QwParameterFile::TrimWhitespace(TString::EStripType head_tail)
{
  //  If the first bit is set, this routine removes leading spaces from the
  //  line.  If the second bit is set, this routine removes trailing spaces
  //  from the line.  The default behavior is to remove both.
  TrimWhitespace(fLine, head_tail);
}

void QwParameterFile::Trim(
	const std::string& chars,
	std::string& token,
	TString::EStripType head_tail)
{
  //  If the first bit is set, this routine removes leading chars from the
  //  line.  If the second bit is set, this routine removes trailing chars
  //  from the line.  The default behavior is to remove both.
  size_t mypos;
  //  Remove leading chars.  If this first test returns "npos", it means
  //  this line is all chars, so get rid of it all.
  //  If we're not removing leading chars, lines which are all chars
  //  will not be removed.
  mypos = token.find_first_not_of(chars);
  if (head_tail & TString::kLeading) token.erase(0,mypos);
  //  Remove trailing spaces
  mypos = token.find_last_not_of(chars);
  mypos = token.find_first_of(chars,mypos);
  if (mypos != std::string::npos && (head_tail & TString::kTrailing)){
    token.erase(mypos);
  }
}

void QwParameterFile::TrimWhitespace(
	std::string &token,
	TString::EStripType head_tail)
{
  Trim(fWhitespaceChars,token,head_tail);
}

void QwParameterFile::TrimComment(const char commentchar)
{
  std::string commentchars = ""; // no std::string c'tor from single char
  commentchars += commentchar; // so we have to use the operator+= overload
  TrimComment(commentchars);
}

void QwParameterFile::TrimComment(const std::string& commentchars)
{
  // Remove everything after the comment character
  size_t mypos = fLine.find_first_of(commentchars);
  if (mypos != std::string::npos){
    fLine.erase(mypos);
  }
}

void QwParameterFile::TrimSectionHeader()
{
  // Trim section delimiter character on both sides
  Trim(fSectionChars,fLine,TString::kBoth);
}

void QwParameterFile::TrimModuleHeader()
{
  // Trim module delimiter character on both sides
  Trim(fModuleChars,fLine,TString::kBoth);
}


Bool_t QwParameterFile::HasValue(TString& vname)
{
  Bool_t status=kFALSE;
  TString vline;

  RewindToFileStart();
  while (ReadNextLine()){
    TrimWhitespace();
    vline=(GetLine()).c_str();

    vline.ToLower();
    TRegexp regexp(vline);
    vname.ToLower();
    if (vname.Contains(regexp)){
      QwMessage <<" QwParameterFile::HasValue  "<<vline<<" "<<vname<<QwLog::endl;
      status=kTRUE;
      break;
    }
  }

  return status;
}


Bool_t QwParameterFile::HasVariablePair(
	const std::string& separatorchars,
	TString &varname,
	TString &varvalue)
{
  std::string tmpvar, tmpval;
  Bool_t status = HasVariablePair(separatorchars, tmpvar, tmpval);
  if (status){
    varname  = tmpvar.c_str();
    varvalue = tmpval.c_str();
  }
  return status;
}

Bool_t QwParameterFile::HasVariablePair(
	const std::string& separatorchars,
	std::string &varname,
	std::string &varvalue)
{
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of(separatorchars);
  if (equiv_pos1 != std::string::npos){
    size_t equiv_pos2 = fLine.find_first_not_of(separatorchars,equiv_pos1);
    if (equiv_pos2 != std::string::npos){
      varname  = fLine.substr(0,equiv_pos1);
      varvalue = fLine.substr(equiv_pos2);
      TrimWhitespace(varname,  TString::kBoth);
      TrimWhitespace(varvalue, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
}

Bool_t QwParameterFile::FileHasVariablePair(
	const std::string& separatorchars,
	const TString& varname,
	TString& varvalue)
{
  std::string tmpval;
  Bool_t status = FileHasVariablePair(separatorchars, varname.Data(), tmpval);
  if (status) varvalue = tmpval.c_str();
  return status;
}

Bool_t QwParameterFile::FileHasVariablePair(
	const std::string& separatorchars,
	const std::string& varname,
	std::string& varvalue)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_varname;
    if (HasVariablePair(separatorchars, this_varname, varvalue)) {
      if (this_varname == varname) return kTRUE;
    }
  }
  return false;
}


Bool_t QwParameterFile::LineHasSectionHeader()
{
  std::string secname;
  return LineHasSectionHeader(secname);
}

Bool_t QwParameterFile::LineHasSectionHeader(TString& secname)
{
  std::string secname_tmp = secname.Data();
  Bool_t status = LineHasSectionHeader(secname_tmp);
  secname = secname_tmp;
  return status;
}

Bool_t QwParameterFile::LineHasSectionHeader(std::string& secname)
{
  TrimComment();
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of(fSectionChars[0]);
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_last_of(fSectionChars[1]);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
}

Bool_t QwParameterFile::LineHasModuleHeader()
{
  std::string secname;
  return LineHasModuleHeader(secname);
}

Bool_t QwParameterFile::LineHasModuleHeader(TString& secname)
{
  std::string secname_tmp = secname.Data();
  Bool_t status = LineHasModuleHeader(secname_tmp);
  secname = secname_tmp;
  return status;
}

Bool_t QwParameterFile::LineHasModuleHeader(std::string& secname)
{
  TrimComment();
  Bool_t status = kFALSE;
  size_t equiv_pos1 = fLine.find_first_of(fModuleChars[0]);
  if (equiv_pos1 != std::string::npos) {
    size_t equiv_pos2 = fLine.find_last_of(fModuleChars[1]);
    if (equiv_pos2 != std::string::npos && equiv_pos2 - equiv_pos1 > 1) {
      secname = fLine.substr(equiv_pos1 + 1, equiv_pos2 - equiv_pos1 - 1);
      TrimWhitespace(secname, TString::kBoth);
      status = kTRUE;
    }
  }
  return status;
}


Bool_t QwParameterFile::FileHasSectionHeader(const TString& secname)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasSectionHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
}

Bool_t QwParameterFile::FileHasSectionHeader(const std::string& secname)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasSectionHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
}

Bool_t QwParameterFile::FileHasModuleHeader(const TString& secname)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasModuleHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
}

Bool_t QwParameterFile::FileHasModuleHeader(const std::string& secname)
{
  RewindToFileStart();
  while (ReadNextLine()) {
    std::string this_secname;
    if (LineHasModuleHeader(this_secname)) {
      if (this_secname == secname) return kTRUE;
    }
  }
  return false;
}

/**
 * Read from current position until next section header
 * @return Pointer to the parameter stream until next section
 */
QwParameterFile* QwParameterFile::ReadUntilNextSection(const bool add_current_line)
{
  std::string nextheader; // dummy
  QwParameterFile* section = new QwParameterFile();
  if (add_current_line) section->AddLine(GetLine()); // add current line
  while (ReadNextLine() && ! LineHasSectionHeader(nextheader)) {
    section->AddLine(GetLine());
  }
  return section;
}

/**
 * Read from current position until next module header
 * @return Pointer to the parameter stream until next module
 */
QwParameterFile* QwParameterFile::ReadUntilNextModule(const bool add_current_line)
{
  std::string nextheader; // dummy
  QwParameterFile* section = new QwParameterFile();
  if (add_current_line) section->AddLine(GetLine()); // add current line
  while (ReadNextLine() && ! LineHasModuleHeader(nextheader)) {
    section->AddLine(GetLine());
  }
  return section;
}

Bool_t QwParameterFile::SkipSection(std::string secname)
{
  //  If the current line begins the section to be skipped,
  //  just keep reading until we get to the next section.
  //  Recurse, just in case the next section is the same
  //  section name.
  Bool_t status = kTRUE;
  std::string tmpname;
  if (LineHasSectionHeader(tmpname)){
    if (tmpname == secname){
      QwDebug << "QwParameterFile::SkipSection:  "
	      << "Begin skipping section " << tmpname << QwLog::endl;
      while ((status=ReadNextLine()) && ! LineHasSectionHeader(tmpname)) {
	//  Do nothing for each line.
      }
      QwDebug << "QwParameterFile::SkipSection:  "
	      << "Reached the end of the section." 
	      << QwLog::endl;
      if (status){
	// Recurse, in case the next section has the same
	// section name, but only if we found a new section
	// header.
	status = SkipSection(secname);
      }
    }
  }
  return status;
}


/**
 * Read the lines until the first header
 * @return Pointer to the parameter stream until first section
 */
QwParameterFile* QwParameterFile::ReadSectionPreamble()
{
  RewindToFileStart();
  return ReadUntilNextSection();
}

/**
 * Read the lines of the next section
 * @param secname Name of the next section (returns)
 * @param keep_header Keep the header inside the section
 * @return Pointer to the parameter stream of the next section
 */
QwParameterFile* QwParameterFile::ReadNextSection(std::string &secname, const bool keep_header)
{
  if (IsEOF()) return 0;
  while (! LineHasSectionHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextSection(keep_header);
}

QwParameterFile* QwParameterFile::ReadNextSection(TString &secname, const bool keep_header)
{
  if (IsEOF()) return 0;
  while (! LineHasSectionHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextSection(keep_header);
}


/**
 * Read the lines until the first header
 * @return Pointer to the parameter stream until first module
 */
QwParameterFile* QwParameterFile::ReadModulePreamble()
{
  RewindToFileStart();
  return ReadUntilNextModule();
}

/**
 * Read the lines of the next module
 * @param secname Name of the next module (returns)
 * @param keep_header Flag to keep header of module
 * @return Pointer to the parameter stream of the next module
 */
QwParameterFile* QwParameterFile::ReadNextModule(std::string &secname, const bool keep_header)
{
  if (IsEOF()) return 0;
  while (! LineHasModuleHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextModule(keep_header);
}

QwParameterFile* QwParameterFile::ReadNextModule(TString &secname, const bool keep_header)
{
  if (IsEOF()) return 0;
  while (! LineHasModuleHeader(secname) && ReadNextLine()); // skip until header
  return ReadUntilNextModule(keep_header);
}

std::string QwParameterFile::GetNextToken(const std::string& separatorchars)
{
  std::string tmpstring = "";
  if (fCurrentPos != std::string::npos){
    size_t pos1 = fCurrentPos;
    size_t pos2 = fLine.find_first_of(separatorchars.c_str(), pos1);
    if (pos2 == std::string::npos){
      fCurrentPos = pos2;
      tmpstring   = fLine.substr(pos1);
    } else {
      fCurrentPos = fLine.find_first_not_of(separatorchars.c_str(), pos2);
      tmpstring   = fLine.substr(pos1,pos2-pos1);
    }
  }
  TrimWhitespace(tmpstring,TString::kBoth);
  return tmpstring;
}

std::ostream& operator<< (std::ostream& stream, const QwParameterFile& file)
{
  /// \todo TODO (wdc) operator<< on QwParameterFile requires RewindToFileStart
  std::string line;
  stream << file.fStream.rdbuf();
  return stream;
}


/** @brief Interpret an integer with optional scale factor
 *
 *  @param value String containing an integer with optional scale factor
 *
 *  @return Pure integer value with any scale factor applied
 */
int QwParameterFile::ParseInt(const std::string& value)
{
  int retval;

  const std::string scalechars("kM");
  size_t pos = value.find_first_of(scalechars);
  if (pos == std::string::npos) {
    //  Separator not found.
    retval = atoi(value.c_str());
  } else if (pos == value.length()-1) {
    retval = atoi(value.substr(0,pos).c_str());
    switch (value[pos]) {
      case 'k': retval *= 1e3; break;
      case 'M': retval *= 1e6; break;
      default:
        QwError << "Integer contains unknown scale factor! " << value << QwLog::endl;
        break;
    }
  } else {
    QwError << "Integer must end with scale factor! " << value << QwLog::endl;
    return INT_MAX;
  }

  //  Print the integer for debugging.
  QwVerbose << "The integer is " << retval << QwLog::endl;

  return retval;
}


/** @brief Separate a separated range of integers into a pair of values
 *
 *  @param separatorchars String with possible separator characters to consider.
 *  @param range String containing two integers separated by a separator character,
 *               or a single value.
 *               If the string begins with the separator character, the first value
 *               is taken as zero.  If the string ends with the separator character,
 *               the second value is taken as kMaxInt.
 *
 *  @return  Pair of integers of the first and last values of the range.
 *           If the range contains a single value, the two integers will
 *           be identical.
 */
std::pair<int,int> QwParameterFile::ParseIntRange(const std::string& separatorchars, const std::string& range)
{
  std::pair<int,int> mypair;
  size_t pos = range.find_first_of(separatorchars);
  if (pos == std::string::npos) {
    //  Separator not found.
    mypair.first  = ParseInt(range.substr(0,range.length()).c_str());
    mypair.second = mypair.first;
  } else {
    size_t end = range.length() - pos - 1;
    if (pos == 0) {
      // Separator is the first character
      mypair.first  = 0;
      mypair.second = ParseInt(range.substr(pos+1, end).c_str());
    } else if (pos == range.length() - 1) {
      // Separator is the last character
      mypair.first  = ParseInt(range.substr(0,pos).c_str());
      mypair.second = INT_MAX;
    } else {
      mypair.first  = ParseInt(range.substr(0,pos).c_str());
      mypair.second = ParseInt(range.substr(pos+1, end).c_str());
    }
  }

  //  Check the values for common errors.
  if (mypair.first < 0){
    QwError << "The first value must not be negative!" << QwLog::endl;
    return std::pair<int,int>(INT_MAX,INT_MAX);
  } else if (mypair.first > mypair.second){
    QwError << "The first value ("<< mypair.first<< ") must not be larger than the second value (" << mypair.second <<")"
	    << QwLog::endl;
    return std::pair<int,int>(INT_MAX,INT_MAX);
  }

  //  Print the contents of the pair for debugging.
  QwVerbose << "The range goes from " << mypair.first
	    << " to " << mypair.second << QwLog::endl;

  return mypair;
}



void QwParameterFile::SetParamFilename() 
{
  Char_t delimiters[] = "/";
  fBestParamFileName = LastString(fBestParamFileNameAndPath, delimiters);  
  return;
};
 

TString QwParameterFile::LastString(TString in, char* delim)
{	
  TObjArray*  all_strings = in.Tokenize(delim); 
  TObjString* last_string = (TObjString*) all_strings->Last();
  TString return_string = last_string->GetString();
  delete all_strings;
  return return_string;
};
 

TString QwParameterFile::GetParameterFileContents()
{
  TMacro *fParameterFile = new TMacro(fBestParamFileNameAndPath);
  TString ms;
  TList *list = fParameterFile->GetListOfLines();
  for(Int_t i=0; i < list->GetSize(); i++) {
    ms += list->At(i)->GetName();
    ms += "\n";
  }
  delete fParameterFile;
  fParameterFile = NULL;
  return ms;
}


void  QwParameterFile::AddBreakpointKeyword(std::string keyname){
  std::transform(keyname.begin(), keyname.end(),
		 keyname.begin(), ::tolower);
  fBreakpointWords.insert(keyname);
}

Bool_t QwParameterFile::ReadNextLine_Greedy(std::string &varvalue)
{
  //  Keep reading until a non-comment, non-empty,
  //  non-keyword-value-pair line has been found.
  Bool_t status;
  std::string tmpname, tmpvalue;
  while ((status = ReadNextLine_Single(varvalue))){
    TrimComment();
    TrimWhitespace();
    if (LineIsEmpty()) continue;
    if (HasVariablePair("=",tmpname,tmpvalue)){
      std::transform(tmpname.begin(), tmpname.end(),
		     tmpname.begin(), ::tolower);
      QwDebug << "QwParameterFile::ReadNextLine_Greedy: varname="
	      << tmpname << "; varvalue=" << tmpvalue <<QwLog::endl;
      if (fBreakpointWords.count(tmpname)==1){
	RewindToLineStart();
	break;
      } else {
	fKeyValuePair[tmpname] = tmpvalue;
	fHasNewPairs = kTRUE;
	continue;
      }
    }
    break;
  }
  return status;
}
