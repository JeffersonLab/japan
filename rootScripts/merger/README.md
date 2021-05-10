Smart HADD is a ROOT "hadd" replacement script. 
  Notably, it will take the final entry passed in the listOfFiles text list and will use its device list to define the allowed devices for all prior entries. 
  It will sort by index1 and index2 using a ROOT sorting method.
  It will fill with empties (Double_t 0.0) without erroring on all branches that don't exist in prior entrys but which do exist in the final entry (the one that defines the available branch list).

smartHadd(std::string listOfFiles = "slug_list.txt", std::string inputFolderName = "/chafs2/work1/apar/aggRootfiles/slugRootfiles/", std::string outputFileName = "grand_aggregator.root", std::string index1 = "n_runs", std::string index2 = "run_number", std::string tree = "agg") {

  This Smart hadd replacement will do a standard tree TChaining, on multiple trees within one ROOT file");
    Arg 1: Default = "./slug_list.txt" - The input list of integers, representing the input file indices");
           You should fill a file with a list of the indices, in preferred order, but index sorting is done later");
           Note that the final index is the file which will be used to determine the final Tree's branch list.");
           Any missing branches from initial indexed files will be filled with zeros.");
    Arg 2: Default = "/adaqfs/home/apar/PREX/japan/rootScripts/merger/input_smartHadd.txt" - The config file path");
           You should fill an input text file, the 3rd argument, with several optional arguments:");
      "ROOT input path":      The input path from which you would like to select data, with a # symbol in place of the integer identifier");
      "Rootfile Output Path": Can be used to override argument 3, the output path destination");
      "Trees":                The list of Tree Names you would like to hadd independently");
      "Index 1":              The major sorting Tree Index, default is "run_number" - must be an integer, or round to a unique indexable integer");
      "Index 2":              The minor sorting Tree Index, default is "minirun_n" - must be an integer, or round to a unique indexable integer");
    Arg 3: Default = "./grand_aggregator.root" - The output file path\n");
