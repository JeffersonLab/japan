#include "toolbox.hh"
#include "tools.hh"
int toolbox(){
  TStopwatch tsw;
  Printf("Starting ToolBox Analysis");
  tsw.Start();
  ToolBox * tb = new ToolBox();

  //// A test case: tb->eigenvector_averaging("crex_part");
  //tb->dit_slopes_averaging("segment");
  //tb->manage_aggregator_friend_to_mini();
  //tb->manage_dit_copy_and_friend_to_mini();
  tb->manage_postpan_copy_and_friend_to_mini();
  //tb->expand_dit_averaged_slopes();

  cout << "-- ToolBox Analysis done in "; tsw.Print(); cout << endl;
  delete tb;
  return 0;
}
