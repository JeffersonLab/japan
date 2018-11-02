/*!
 * \file   QwOptions.cc
 * \brief  An options class which parses command line, config file and environment
 *
 * \author Wouter Deconinck
 * \date   2009-12-01
 */

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif


#include "QwOptions.h"

// System headers
#include <iostream>
#include <fstream>
#include <cstdlib>
//#include <TROOT.h>

// Globally defined instance of the options object
QwOptions gQwOptions;

// Qweak headers
#include "QwLog.h"
#include "QwParameterFile.h"

// Qweak objects with default options
#include "QwSubsystemArray.h"
#include "QwEventBuffer.h"
#include "QwEPICSEvent.h"
#ifdef  __USE_DATABASE__
#include "QwDatabase.h"
#endif
#include "QwRootFile.h"
#include "QwHistogramHelper.h"

// Initialize the static command line arguments to zero
int QwOptions::fArgc = 0;
char** QwOptions::fArgv = 0;

/**
 * The default constructor sets up the options description object with some
 * options that should always be there.  The other options can be setup
 * wherever this object is accessible.
 */
QwOptions::QwOptions()
  : fParsed(false)
{
  // No default config files
  fConfigFiles.clear();

  // Declare the default options
  AddDefaultOptions()("usage", "print this help message");
  AddDefaultOptions()("help,h", "print this help message");
  AddDefaultOptions()("version,V", "print the version string");
  AddDefaultOptions()("config,c", po::value<std::string>(), "read ONLY this config file\n(will override default config files)");
  AddDefaultOptions()("add-config,a", po::value<std::vector<std::string> >(), "read ALSO this config file\n(will keep the default config files)");
}

/**
 * Define standard options on the specified options object
 * @param options Options object
 */
void QwOptions::DefineOptions(QwOptions& options)
{
  // Define logging options (Note: only QwLog takes a pointer argument!!!)
  QwLog::DefineOptions(&options);

  // Define execution options
  QwEventBuffer::DefineOptions(options);
#ifdef __USE_DATABASE__
  // Define database options
  QwDatabase::DefineOptions(options);
#endif //__USE_DATABASE__
  // Define ROOT file options
  QwRootFile::DefineOptions(options);
  // Define EPICS event options
  QwEPICSEvent::DefineOptions(options);
  // Define subsystem array options
  QwSubsystemArray::DefineOptions(options);
  // Define histogram helper options
  QwHistogramHelper::DefineOptions(options);
}

/**
 * Destructor where we clean up locally allocated memory
 */
QwOptions::~QwOptions()
{
  // Clean up the copy of the command line arguments
  // Note: only the array of arguments is allocated, the arguments themselves
  // are still owned by main.
  if (fArgv)
    delete[] fArgv;

  // Delete the option blocks
  for (size_t i = 0; i < fOptionBlock.size(); i++)
    delete fOptionBlock.at(i);
  fOptionBlock.clear();
}

/**
 * Make a local copy of the command line arguments so they are available
 * for later parsing.
 * @param argc Number of arguments
 * @param argv[] Array of arguments
 * @param default_config_file Flag to enable default config file named by executable
 */
void QwOptions::SetCommandLine(int argc, char* argv[], bool default_config_file)
{
  // Copy command line options
  fArgc = argc;
  if (fArgv) delete[] fArgv;
  fArgv = new char*[fArgc];
  QwDebug << "Arguments:";
  for (int i = 0; i < argc; i++) {
    fArgv[i] = argv[i];
    QwDebug << " " << fArgv[i];
  }
  QwDebug << QwLog::endl;

  fParsed = false;

  // Add default config file based on file name
  if (fArgc > 0 && default_config_file) {
    std::string path = fArgv[0];
    QwDebug << "Invocation name: " << path << QwLog::endl;
    // Find file name from full path
    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos)
      // Called with path
      AddConfigFile(path.substr(pos+1) + ".conf");
    else
      // Called without path
      AddConfigFile(path + ".conf");
  }
}



/**
 * Combine the options of the various option descriptions in one object for
 * parsing at once.  This avoids having to try/catch every single option
 * description
 */
po::options_description* QwOptions::CombineOptions()
{
  // The options can be grouped by defining a vector fOptions of
  // options_description objects. Each entry can have a name and
  // will show up as a separate section in the usage information.
  po::options_description* options = new po::options_description("options");
  for (size_t i = 0; i < fOptionBlockName.size(); i++) {
    // Right now every parser gets access to all options
    options->add(*fOptionBlock.at(i));
  }
  return options;
}

/**
 * Parse the command line arguments for options and warn when encountering
 * an unknown option, then notify the variables map.  Print usage instructions
 * when no options are given, or when explicitly asked for.
 */
void QwOptions::ParseCommandLine()
{
#if BOOST_VERSION >= 103300
  //  Boost versions starting with 1.33.00 allow unrecognized options to be
  //  passed through the parser.
  try {
    po::options_description* command_line_options = CombineOptions();
    po::store(po::command_line_parser(fArgc, fArgv).options(*command_line_options).allow_unregistered().run(), fVariablesMap);
    delete command_line_options;
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing command line arguments" << QwLog::endl;
    exit(10);
  }
#endif

#if BOOST_VERSION < 103300
  //  Boost versions before 1.33.00 do not allow unrecognized options to be
  //  passed through the parser.
  try {
    //  Boost versions before 1.33.00 do not recognize "allow_unregistered".
    po::options_description* command_line_options = CombineOptions();
    po::store(po::command_line_parser(fArgc, fArgv).options(*command_line_options).run(), fVariablesMap);
    delete command_line_options;
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing command line arguments" << QwLog::endl;
    QwWarning << "All command line arguments may have been ignored!" << QwLog::endl;
    exit(10);
  }
#endif

  // Notify of new options
  po::notify(fVariablesMap);

  // If option help/usage, print help text
  if (fVariablesMap.count("help") || fVariablesMap.count("usage")) {
    Usage();
    exit(0);
  }

  // If option version, print version string
  if (fVariablesMap.count("version")) {
    Version();
    exit(0);
  }

  // If a configuration file is specified, load it.
  if (fVariablesMap.count("config") > 0) {
    QwWarning << "Overriding the default configuration files with "
	      << "user-defined configuration file "
              << fVariablesMap["config"].as<std::string>() << QwLog::endl;
    SetConfigFile(fVariablesMap["config"].as<std::string>());
  }
  // If a configuration file is specified, load it.
  if (fVariablesMap.count("add-config") > 0) {
    QwWarning << "Adding user-defined configuration file " << QwLog::endl;
    AddConfigFile(fVariablesMap["add-config"].as<std::vector<std::string> >());
  }
}

/**
 * Parse the environment variables for options and notify the variables map.
 */
void QwOptions::ParseEnvironment()
{
  try {
    po::options_description* environment_options = CombineOptions();
    po::store(po::parse_environment(*environment_options, "Qw"), fVariablesMap);
    delete environment_options;
  } catch (std::exception const& e) {
    QwWarning << e.what() << " while parsing environment variables" << QwLog::endl;
    exit(10);
  }
  // Notify of new options
  po::notify(fVariablesMap);
}

/**
 * Parse the configuration file for options and warn when encountering
 * an unknown option, then notify the variables map.
 */
void QwOptions::ParseConfigFile()
{
  for (size_t i = 0; i < fConfigFiles.size(); i++) {
    QwParameterFile configfile(fConfigFiles.at(i).c_str());
    std::stringstream configstream;
    configstream << configfile.rdbuf();

    try {
#if BOOST_VERSION >= 103500
      // Boost version after 1.35 have bool allow_unregistered = false in
      // their signature.  This allows for unknown options in the config file.
      po::options_description* config_file_options = CombineOptions();
      po::store(po::parse_config_file(configstream, *config_file_options, true),
		fVariablesMap);
      delete config_file_options;
#else
      // Boost versions before 1.35 cannot handle files with unregistered
      // options.
      po::options_description* config_file_options = CombineOptions();
      po::store(po::parse_config_file(configstream, *config_file_options),
		fVariablesMap);
      delete config_file_options;
#endif
    } catch (std::exception const& e) {
      QwWarning << e.what() << " while parsing configuration file "
                << fConfigFiles.at(i) << QwLog::endl;
#if BOOST_VERSION < 103500
      QwWarning << "The entire configuration file was ignored!" << QwLog::endl;
#endif
      exit(10);
    }
    // Notify of new options
    po::notify(fVariablesMap);
  }
}



/**
 * Print usage information
 */
void QwOptions::Usage()
{
  QwMessage << QwLog::endl;
  QwMessage << "Welcome to the Qweak analyzer code." << QwLog::endl;
  QwMessage << QwLog::endl;
  for (size_t i = 0; i < fOptionBlock.size(); i++)
    QwMessage << *(fOptionBlock.at(i)) << QwLog::endl;
}


/**
 * Print version string
 */
void QwOptions::Version()
{
  TString root_version = gROOT->GetVersion();
  root_version += ", Date : ";
  root_version += gROOT->GetVersionDate();
#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
  root_version += ", SVN : ";
  root_version += gROOT->GetSvnRevision();
  root_version += " ";
  root_version += gROOT->GetSvnBranch();
#else // ROOT_VERSION_CODE >= ROOT_VERSION(6,0,0)
  root_version += ", GIT : ";
  root_version += gROOT->GetGitCommit();
  root_version += " ";
  root_version += gROOT->GetGitBranch();
#endif

  QwMessage << "\n Qweak Analysis Framework : " << fArgv[0] << QwLog::endl;
  //  QwMessage << " * Revision: " << QWANA_SVN_REVISION << QwLog::endl;
  //  QwMessage << " * URL: " << QWANA_SVN_URL << QwLog::endl;
  //  QwMessage << " * Last Changed Rev: " << QWANA_SVN_LASTCHANGEDREVISION << QwLog::endl;
  QwMessage << " * ROOT " << root_version << QwLog::endl;
}


/**
 * Get a pair of integers specified as a colon-separated range
 * This function uses the utility function QwParameterFile::ParseIntRange.
 * @param key Option key
 * @return Pair of integers
 */
std::pair<int,int> QwOptions::GetIntValuePair(const std::string& key)
{
  std::pair<int, int> mypair;
  mypair.first = 0;
  mypair.second = 0;

  if (fParsed == false) Parse();
  if (fVariablesMap.count(key)) {
    std::string range = fVariablesMap[key].as<std::string>();
    mypair = QwParameterFile::ParseIntRange(":",range);
  }

  return mypair;
}
