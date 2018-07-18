
#include "src/Traits.h"
#include "src/Run/Run.h"
#include "test/interface/ModelAuxControl.h"

int main(int argc,  char ** argv) {
  soca::Run run(argc, argv);
  test::ModelAuxControl<soca::Traits> tests;
  run.execute(tests);
  return 0;
};

