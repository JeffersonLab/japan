#ifndef __QWROOTFILE__
#define __QWROOTFILE__

// System headers
#include <typeindex>
#include <unistd.h>
using std::type_info;

// ROOT headers
#include "TFile.h"
#include "TTree.h"
#include "TPRegexp.h"
#include "TSystem.h"

// Qweak headers
#include "QwOptions.h"
#include "TMapFile.h"


// If one defines more than this number of words in the full ntuple,
// the results are going to get very very crazy.
#define BRANCH_VECTOR_MAX_SIZE 13000


/**
 *  \class QwRootTree
 *  \ingroup QwAnalysis
 *  \brief A wrapper class for a ROOT tree
 *
 * This class provides the functionality to write to ROOT trees using a vector
 * of doubles.  The vector is part of this object, as well as a pointer to the
 * tree that contains the branches.  One ROOT tree can have multiple QwRootTree
 * objects, for example in tracking mode both parity and tracking detectors
 * can be stored in the same tree.
 */
class QwRootTree {

  public:

    /// Constructor with name, and description
    QwRootTree(const std::string& name, const std::string& desc, const std::string& prefix = "")
    : fName(name),fDesc(desc),fPrefix(prefix),fType("type undefined"),
      fCurrentEvent(0),fNumEventsCycle(0),fNumEventsToSave(0),fNumEventsToSkip(0) {
      // Construct tree
      ConstructNewTree();
    }

    /// Constructor with existing tree
    QwRootTree(const QwRootTree* tree, const std::string& prefix = "")
    : fName(tree->GetName()),fDesc(tree->GetDesc()),fPrefix(prefix),fType("type undefined"),
      fCurrentEvent(0),fNumEventsCycle(0),fNumEventsToSave(0),fNumEventsToSkip(0) {
      QwMessage << "Existing tree: " << tree->GetName() << ", " << tree->GetDesc() << QwLog::endl;
      fTree = tree->fTree;
    }

    /// Constructor with name, description, and object
    template < class T >
    QwRootTree(const std::string& name, const std::string& desc, T& object, const std::string& prefix = "")
    : fName(name),fDesc(desc),fPrefix(prefix),fType("type undefined"),
      fCurrentEvent(0),fNumEventsCycle(0),fNumEventsToSave(0),fNumEventsToSkip(0) {
      // Construct tree
      ConstructNewTree();

      // Construct branch
      ConstructUnitsBranch();

      // Construct branches and vector
      ConstructBranchAndVector(object);
    }

    /// Constructor with existing tree, and object
    template < class T >
    QwRootTree(const QwRootTree* tree, T& object, const std::string& prefix = "")
    : fName(tree->GetName()),fDesc(tree->GetDesc()),fPrefix(prefix),fType("type undefined"),
      fCurrentEvent(0),fNumEventsCycle(0),fNumEventsToSave(0),fNumEventsToSkip(0) {
      QwMessage << "Existing tree: " << tree->GetName() << ", " << tree->GetDesc() << QwLog::endl;
      fTree = tree->fTree;

      // Construct branches and vector
      ConstructBranchAndVector(object);
    }

    /// Destructor
    virtual ~QwRootTree() { }


  private:

    static const TString kUnitsName;
    static Double_t kUnitsValue[];

    /// Construct the tree
    void ConstructNewTree() {
      QwMessage << "New tree: " << fName << ", " << fDesc << QwLog::endl;
      fTree = new TTree(fName.c_str(), fDesc.c_str());
    }

    void ConstructUnitsBranch() {
      std::string name = "units";
      fTree->Branch(name.c_str(), &kUnitsValue, kUnitsName);
    }

    /// Construct index from this tree to another tree
    void ConstructIndexTo(QwRootTree* to) {
      std::string name = "previous_entry_in_" + to->fName;
      fTree->Branch(name.c_str(), &(to->fCurrentEvent));
    }

    /// Construct the branches and vector for generic objects
    template < class T >
    void ConstructBranchAndVector(T& object) {
      // Reserve space for the branch vector
      fVector.reserve(BRANCH_VECTOR_MAX_SIZE);
      // Associate branches with vector
      TString prefix = Form("%s",fPrefix.c_str());
      object.ConstructBranchAndVector(fTree, prefix, fVector);

      // Store the type of object
      fType = typeid(object).name();

      // Check memory reservation
      if (fVector.size() > BRANCH_VECTOR_MAX_SIZE) {
        QwError << "The branch vector is too large: " << fVector.size() << " leaves!  "
                << "The maximum size is " << BRANCH_VECTOR_MAX_SIZE << "."
                << QwLog::endl;
        exit(-1);
      }
    }
   

  public:

    /// Fill the branches for generic objects
    template < class T >
    void FillTreeBranches(const T& object) {
      if (typeid(object).name() == fType) {
        // Fill the branch vector
        object.FillTreeVector(fVector);
      } else {
        QwError << "Attempting to fill tree vector for type " << fType << " with "
                << "object of type " << typeid(object).name() << QwLog::endl;
        exit(-1);
      }
    }

    Long64_t AutoSave(Option_t *option){
      return fTree->AutoSave(option);
    }

    /// Fill the tree
    Int_t Fill() {
      fCurrentEvent++;

      // Tree prescaling
      if (fNumEventsCycle > 0) {
        fCurrentEvent %= fNumEventsCycle;
        if (fCurrentEvent > fNumEventsToSave)
          return 0;
      }

      // Fill the tree
      Int_t retval = fTree->Fill();
      // Check for errors
      if (retval < 0) {
        QwError << "Writing tree failed!  Check disk space or quota." << QwLog::endl;
        exit(retval);
      }
      return retval;
    }


    /// Print the tree name and description
    void Print() const {
      QwMessage << GetName() << ", " << GetType();
      if (fPrefix != "")
        QwMessage << " (prefix " << GetPrefix() << ")";
      QwMessage << QwLog::endl;
    }

    /// Get the tree pointer for low level operations
    TTree* GetTree() const { return fTree; };


  friend class QwRootFile;

  private:

    /// Tree pointer
    TTree* fTree;
    /// Vector of leaves
    std::vector<Double_t> fVector;


    /// Name, description
    const std::string fName;
    const std::string fDesc;
    const std::string fPrefix;

    /// Get the name of the tree
    const std::string& GetName() const { return fName; };
    /// Get the description of the tree
    const std::string& GetDesc() const { return fDesc; };
    /// Get the description of the tree
    const std::string& GetPrefix() const { return fPrefix; };


    /// Object type
    std::string fType;

    /// Get the object type
    std::string GetType() const { return fType; };


    /// Tree prescaling parameters
    UInt_t fCurrentEvent;
    UInt_t fNumEventsCycle;
    UInt_t fNumEventsToSave;
    UInt_t fNumEventsToSkip;

    /// Set tree prescaling parameters
    void SetPrescaling(UInt_t num_to_save, UInt_t num_to_skip) {
      fNumEventsToSave = num_to_save;
      fNumEventsToSkip = num_to_skip;
      fNumEventsCycle = fNumEventsToSave + fNumEventsToSkip;
    }


    /// Maximum tree size, autoflush and autosave
    Long64_t fMaxTreeSize;
    Long64_t fAutoFlush;
    Long64_t fAutoSave;
    Int_t fBasketSize;

    /// Set maximum tree size
    void SetMaxTreeSize(Long64_t maxsize = 1900000000) {
      fMaxTreeSize = maxsize;
      if (fTree) fTree->SetMaxTreeSize(maxsize);
    }

    /// Set autoflush size
    void SetAutoFlush(Long64_t autoflush = 30000000) {
      fAutoFlush = autoflush;
      #if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
        if (fTree) fTree->SetAutoFlush(autoflush);
      #endif
    }

    /// Set autosave size
    void SetAutoSave(Long64_t autosave = 300000000) {
      fAutoSave = autosave;
      if (fTree) fTree->SetAutoSave(autosave);
    }

    /// Set basket size
    void SetBasketSize(Int_t basketsize = 16000) {
      fBasketSize = basketsize;
      if (fTree) fTree->SetBasketSize("*",basketsize);
    }

    //Set circular buffer size for the memory resident tree
    void SetCircular(Long64_t buff = 100000) {
      if (fTree) fTree->SetCircular(buff);
    }
};



/**
 *  \class QwRootFile
 *  \ingroup QwAnalysis
 *  \brief A wrapper class for a ROOT file or memory mapped file
 *
 * This class functions as a wrapper around a ROOT TFile or a TMapFile.  The
 * common inheritance of both is only TObject, so there is a lot that we have
 * to wrap (rather than inherit).  Theoretically you could have both a TFile
 * and a TMapFile represented by an object of this class at the same time, but
 * that is untested.
 *
 * The functionality of writing to the file is done by templated functions.
 * The objects that are passed to these functions have to provide the following
 * functions:
 * <ul>
 * <li>ConstructHistograms, FillHistograms
 * <li>ConstructBranchAndVector, FillTreeVector
 * </ul>
 *
 * The class keeps track of the registered tree names, and the types of objects
 * that have branches constructed in those trees (via QwRootTree).  In most
 * cases it should be possible to just call FillTreeBranches with only the object,
 * although in rare cases this could be ambiguous.
 *
 * The proper way to register a tree is by either calling ConstructTreeBranches
 * of NewTree first.  Then FillTreeBranches will fill the vector, and FillTree
 * will actually fill the tree.  FillTree should be called only once.
 */
class QwRootFile {

  public:

    /// \brief Constructor with run label
    QwRootFile(const TString& run_label);
    /// \brief Destructor
    virtual ~QwRootFile();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);
    /// \brief Set default ROOT files dir
    static void SetDefaultRootFileDir(const std::string& dir) {
      fDefaultRootFileDir = dir;
    }
    /// \brief Set default ROOT file stem
    static void SetDefaultRootFileStem(const std::string& stem) {
      fDefaultRootFileStem = stem;
    }


    /// Is the ROOT file active?
    Bool_t IsRootFile() const { return (fRootFile); };
    /// Is the map file active?
    Bool_t IsMapFile()  const { return (fMapFile); };

    /// \brief Construct indices from one tree to another tree
    void ConstructIndices(const std::string& from, const std::string& to, bool reverse = true);

    /// \brief Construct the tree branches of a generic object
    template < class T >
    void ConstructTreeBranches(const std::string& name, const std::string& desc, T& object, const std::string& prefix = "");
    /// \brief Fill the tree branches of a generic object by tree name
    template < class T >
    void FillTreeBranches(const std::string& name, const T& object);
    /// \brief Fill the tree branches of a generic object by type only
    template < class T >
    void FillTreeBranches(const T& object);


    template < class T >
      Int_t WriteParamFileList(const TString& name, T& object);


    /// \brief Construct the histograms of a generic object
    template < class T >
    void ConstructObjects(const std::string& name, T& object);

    /// \brief Construct the histograms of a generic object
    template < class T >
    void ConstructHistograms(const std::string& name, T& object);
    /// Fill histograms of the subsystem array
    template < class T >
    void FillHistograms(T& object) {
      // Update regularly
      static Int_t update_count = 0;
      update_count++;
      if ((fUpdateInterval > 0) && ( update_count % fUpdateInterval == 0)) Update();
      if (! HasDirByType(object)) return;
      // Fill histograms
      object.FillHistograms();
    }


    /// Create a new tree with name and description
    void NewTree(const std::string& name, const std::string& desc) {
      if (IsTreeDisabled(name)) return;
      this->cd();
      QwRootTree *tree = 0;
      if (! HasTreeByName(name)) {
        tree = new QwRootTree(name,desc);
      } else {
        tree = new QwRootTree(fTreeByName[name].front());
      }
      fTreeByName[name].push_back(tree);
    }

    /// Get the tree with name
    TTree* GetTree(const std::string& name) {
      if (! HasTreeByName(name)) return 0;
      else return fTreeByName[name].front()->GetTree();
    }

    /// Fill the tree with name
    Int_t FillTree(const std::string& name) {
      if (! HasTreeByName(name)) return 0;
      else return fTreeByName[name].front()->Fill();
    }

    /// Fill all registered trees
    Int_t FillTrees() {
      // Loop over all registered tree names
      Int_t retval = 0;
      std::map< const std::string, std::vector<QwRootTree*> >::iterator iter;
      for (iter = fTreeByName.begin(); iter != fTreeByName.end(); iter++) {
        retval += iter->second.front()->Fill();
      }
      return retval;
    }

    /// Print registered trees
    void PrintTrees() const {
      QwMessage << "Trees: " << QwLog::endl;
      // Loop over all registered tree names
      std::map< const std::string, std::vector<QwRootTree*> >::const_iterator iter;
      for (iter = fTreeByName.begin(); iter != fTreeByName.end(); iter++) {
        QwMessage << iter->first << ": " << iter->second.size()
                  << " objects registered" << QwLog::endl;
        // Loop over all registered objects for this tree
        std::vector<QwRootTree*>::const_iterator tree;
        for (tree = iter->second.begin(); tree != iter->second.end(); tree++) {
          (*tree)->Print();
        }
      }
    }
    /// Print registered histogram directories
    void PrintDirs() const {
      QwMessage << "Dirs: " << QwLog::endl;
      // Loop ove rall registered directories
      std::map< const std::string, TDirectory* >::const_iterator iter;
      for (iter = fDirsByName.begin(); iter != fDirsByName.end(); iter++) {
        QwMessage << iter->first << QwLog::endl;
      }
    }


    /// Write any object to the ROOT file (only valid for TFile)
    template < class T >
    Int_t WriteObject(const T* obj, const char* name, Option_t* option = "", Int_t bufsize = 0) {
      Int_t retval = 0;
      // TMapFile has no suport for WriteObject
      if (fRootFile) retval = fRootFile->WriteObject(obj,name,option,bufsize);
      return retval;
    }


    // Wrapped functionality
    void Update() {
      if (fMapFile) {
        QwMessage << "TMapFile memory resident size: "
                  << ((int*)fMapFile->GetBreakval() - (int*)fMapFile->GetBaseAddr()) *
                     4 / sizeof(int32_t) / 1024 / 1024 << " MiB"
                  << QwLog::endl;
        fMapFile->Update();
      }else{
	// this option will allow for reading the tree during write
	Long64_t nBytes(0);
	for (auto iter = fTreeByName.begin(); iter != fTreeByName.end(); iter++)
	  nBytes += iter->second.front()->AutoSave("SaveSelf");
        
	QwMessage << "TFile saved: "
                  << nBytes/1000000 << "MB (innacurate number)" //FIXME this calculation is innacurate
                  << QwLog::endl;
      }
    }
    void Print()  { if (fMapFile) fMapFile->Print();  if (fRootFile) fRootFile->Print(); }
    void ls()     { if (fMapFile) fMapFile->ls();     if (fRootFile) fRootFile->ls(); }
    void Map()    { if (fRootFile) fRootFile->Map(); }
    void Close()  {
      if (!fMakePermanent) fMakePermanent = HasAnyFilled();
      if (fMapFile) fMapFile->Close();
      if (fRootFile) fRootFile->Close();
    }

    // Wrapped functionality
    Bool_t cd(const char* path = 0) {
      Bool_t status = kTRUE;
      if (fMapFile)  status &= fMapFile->cd(path);
      if (fRootFile) status &= fRootFile->cd(path);
      return status;
    }

    // Wrapped functionality
    TDirectory* mkdir(const char* name, const char* title = "") {
      // TMapFile has no suport for mkdir
      if (fRootFile) return fRootFile->mkdir(name, title);
      else return 0;
    }

    // Wrapped functionality
    Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) {
      Int_t retval = 0;
      // TMapFile has no suport for Write
      if (fRootFile) retval = fRootFile->Write(name, option, bufsize);
      return retval;
    }


  private:

    /// Private default constructor
    QwRootFile();


    /// ROOT file
    TFile* fRootFile;

    /// ROOT files dir
    TString fRootFileDir;
    /// Default ROOT files dir
    static std::string fDefaultRootFileDir;

    /// ROOT file stem
    TString fRootFileStem;
    /// Default ROOT file stem
    static std::string fDefaultRootFileStem;

    /// While the file is open, give it a temporary filename.  Perhaps
    /// change to a permanent name when closing the file.
    TString fPermanentName;
    Bool_t fMakePermanent;
    Bool_t fUseTemporaryFile;

    /// Search for non-empty trees or histograms in the file
    Bool_t HasAnyFilled(void);
    Bool_t HasAnyFilled(TDirectory* d);

    /// Map file
    TMapFile* fMapFile;
    Bool_t fEnableMapFile;
    Int_t fUpdateInterval;
    Int_t fCompressionLevel;
    Int_t fBasketSize;
    Int_t fAutoFlush;
    Int_t fAutoSave;

  

  private:

    /// List of excluded trees
    std::vector< TPRegexp > fDisabledTrees;
    std::vector< TPRegexp > fDisabledHistos;

    /// Add regexp to list of disabled trees names
    void DisableTree(const TString& regexp) {
      fDisabledTrees.push_back(regexp);
    }
    /// Does this tree name match a disabled tree name?
    bool IsTreeDisabled(const std::string& name) {
      for (size_t i = 0; i < fDisabledTrees.size(); i++)
        if (fDisabledTrees.at(i).Match(name)) return true;
      return false;
    }
    /// Add regexp to list of disabled histogram directories
    void DisableHisto(const TString& regexp) {
      fDisabledHistos.push_back(regexp);
    }
    /// Does this histogram directory match a disabled histogram directory?
    bool IsHistoDisabled(const std::string& name) {
      for (size_t i = 0; i < fDisabledHistos.size(); i++)
        if (fDisabledHistos.at(i).Match(name)) return true;
      return false;
    }


  private:

    /// Tree names, addresses, and types
    std::map< const std::string, std::vector<QwRootTree*> > fTreeByName;
    std::map< const void*      , std::vector<QwRootTree*> > fTreeByAddr;
    std::map< const std::type_index , std::vector<QwRootTree*> > fTreeByType;
    // ... Are type_index objects really unique? Let's hope so.

    /// Is a tree registered for this name
    bool HasTreeByName(const std::string& name) {
      if (fTreeByName.count(name) == 0) return false;
      else return true;
    }
    /// Is a tree registered for this type
    template < class T >
    bool HasTreeByType(const T& object) {
      const std::type_index type = typeid(object);
      if (fTreeByType.count(type) == 0) return false;
      else return true;
    }
    /// Is a tree registered for this object
    template < class T >
    bool HasTreeByAddr(const T& object) {
      const void* addr = static_cast<const void*>(&object);
      if (fTreeByAddr.count(addr) == 0) return false;
      else return true;
    }

    /// Directories
    std::map< const std::string, TDirectory* > fDirsByName;
    std::map< const std::string, std::vector<std::string> > fDirsByType;

    /// Is a tree registered for this name
    bool HasDirByName(const std::string& name) {
      if (fDirsByName.count(name) == 0) return false;
      else return true;
    }
    /// Is a directory registered for this type
    template < class T >
    bool HasDirByType(const T& object) {
      std::string type = typeid(object).name();
      if (fDirsByType.count(type) == 0) return false;
      else return true;
    }


  private:

    /// Prescaling of events written to tree
    UInt_t fNumMpsEventsToSkip;
    UInt_t fNumMpsEventsToSave;
    UInt_t fNumHelEventsToSkip;
    UInt_t fNumHelEventsToSave;
    UInt_t fCircularBufferSize;
    UInt_t fCurrentEvent;

    /// Maximum tree size
    static const Long64_t kMaxTreeSize;
    static const Int_t    kMaxMapFileSize;
};

/**
 * Construct the indices from one tree to another tree, and optionally in reverse as well.
 * @param from Name of tree where index will be created
 * @param to Name of tree to which index will point
 * @param reverse Flag to create indices in both direction
 */
inline void QwRootFile::ConstructIndices(const std::string& from, const std::string& to, bool reverse)
{
  // Return if we do not want this tree information
  if (IsTreeDisabled(from)) return;
  if (IsTreeDisabled(to)) return;

  // If the trees are defined
  if (fTreeByName.count(from) > 0 && fTreeByName.count(to) > 0) {

    // Construct index from the first tree to the second tree
    fTreeByName[from].front()->ConstructIndexTo(fTreeByName[to].front());

    // Construct index from the second tree back to the first tree
    if (reverse)
      fTreeByName[to].front()->ConstructIndexTo(fTreeByName[from].front());
  }
}

/**
 * Construct the tree branches of a generic object
 * @param name Name for tree
 * @param desc Description for tree
 * @param object Subsystem array
 * @param prefix Prefix for the tree
 */
template < class T >
void QwRootFile::ConstructTreeBranches(
        const std::string& name,
        const std::string& desc,
        T& object,
        const std::string& prefix)
{
  // Return if we do not want this tree information
  if (IsTreeDisabled(name)) return;

  // Pointer to new tree
  QwRootTree* tree = 0;

  // If the tree does not exist yet, create it
  if (fTreeByName.count(name) == 0) {

    // Go to top level directory
    this->cd();

    // New tree with name, description, object, prefix
    tree = new QwRootTree(name, desc, object, prefix);

    // Settings only relevant for new trees
    if (name == "evt")
      tree->SetPrescaling(fNumMpsEventsToSave, fNumMpsEventsToSkip);
    else if (name == "mul")
      tree->SetPrescaling(fNumHelEventsToSave, fNumHelEventsToSkip);

    #if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
    tree->SetAutoFlush(fAutoFlush);
    #endif
    tree->SetAutoSave(fAutoSave);
    tree->SetBasketSize(fBasketSize);
    tree->SetMaxTreeSize(kMaxTreeSize);

    if (fCircularBufferSize > 0)
      tree->SetCircular(fCircularBufferSize);

  } else {

    // New tree based on existing tree
    tree = new QwRootTree(fTreeByName[name].front(), object, prefix);
  }

   // Add the branches to the list of trees by name, object, type
  const void* addr = static_cast<const void*>(&object);
  const std::type_index type = typeid(object);
  fTreeByName[name].push_back(tree);
  fTreeByAddr[addr].push_back(tree);
  fTreeByType[type].push_back(tree);
}


/**
 * Fill the tree branches of a generic object by name
 * @param name Name for tree
 * @param object Subsystem array
 */
template < class T >
void QwRootFile::FillTreeBranches(
        const std::string& name,
        const T& object)
{
  // If this name has no registered trees
  if (! HasTreeByName(name)) return;
  // If this type has no registered trees
  if (! HasTreeByType(object)) return;

  // Get the address of the object
  const void* addr = static_cast<const void*>(&object);

  // Fill the trees with the correct address
  for (size_t tree = 0; tree < fTreeByAddr[addr].size(); tree++) {
    if (fTreeByAddr[addr].at(tree)->GetName() == name) {
      fTreeByAddr[addr].at(tree)->FillTreeBranches(object);
    }
  }
}


/**
 * Fill the tree branches of a generic object by type only
 * @param object Subsystem array
 */
template < class T >
void QwRootFile::FillTreeBranches(
        const T& object)
{
  // If this address has no registered trees
  if (! HasTreeByAddr(object)) return;

  // Get the address of the object
  const void* addr = static_cast<const void*>(&object);

  // Fill the trees with the correct address
  for (size_t tree = 0; tree < fTreeByAddr[addr].size(); tree++) {
    fTreeByAddr[addr].at(tree)->FillTreeBranches(object);
  }
}


/**
 * Construct the objects directory of a generic object
 * @param name Name for objects directory
 * @param object Subsystem array
 */
template < class T >
void QwRootFile::ConstructObjects(const std::string& name, T& object)
{
  // Create the objects in a directory
  if (fRootFile) {
    std::string type = typeid(object).name();
    fDirsByName[name] =
        fRootFile->GetDirectory(("/" + name).c_str()) ?
            fRootFile->GetDirectory(("/" + name).c_str()) :
            fRootFile->GetDirectory("/")->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    object.ConstructObjects(fDirsByName[name]);
  }

  // No support for directories in a map file
  if (fMapFile) {
    QwMessage << "QwRootFile::ConstructObjects::detectors address "
	      << &object
	      << " and its name " << name
	      << QwLog::endl;

    std::string type = typeid(object).name();
    fDirsByName[name] = fMapFile->GetDirectory()->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    object.ConstructObjects();
  }
}


/**
 * Construct the histogram of a generic object
 * @param name Name for histogram directory
 * @param object Subsystem array
 */
template < class T >
void QwRootFile::ConstructHistograms(const std::string& name, T& object)
{
  // Return if we do not want this histogram information
  if (IsHistoDisabled(name)) return;

  // Create the histograms in a directory
  if (fRootFile) {
    std::string type = typeid(object).name();
    fDirsByName[name] =
        fRootFile->GetDirectory(("/" + name).c_str()) ?
            fRootFile->GetDirectory(("/" + name).c_str()) :
            fRootFile->GetDirectory("/")->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    object.ConstructHistograms(fDirsByName[name]);
  }

  // No support for directories in a map file
  if (fMapFile) {
    QwMessage << "QwRootFile::ConstructHistograms::detectors address "
	      << &object  
	      << " and its name " << name 
	      << QwLog::endl;

    std::string type = typeid(object).name();
    fDirsByName[name] = fMapFile->GetDirectory()->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    //object.ConstructHistograms(fDirsByName[name]);
    object.ConstructHistograms();
  }
}


template < class T >
Int_t QwRootFile::WriteParamFileList(const TString &name, T& object)
{
  Int_t retval = 0;
  if (fRootFile) {
    TList *param_list = (TList*) fRootFile->FindObjectAny(name);
    if (not param_list) {
      retval = fRootFile->WriteObject(object.GetParamFileNameList(name), name);
    }
  }
  return retval;
}


#endif // __QWROOTFILE__
