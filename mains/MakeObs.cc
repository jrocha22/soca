
#include "model/instantiateObsFactory.h"
#include "model/Traits.h"
#include "oops/runs/MakeObs.h"
#include "oops/runs/Run.h"

int main(int argc,  char ** argv) {
  oops::Run run(argc, argv);
  mom5cice5::instantiateObsFactory();
  oops::MakeObs<mom5cice5::Traits> mkobs;
  run.execute(mkobs);
  return 0;
};
