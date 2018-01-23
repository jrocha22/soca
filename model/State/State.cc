
#include "model/State/State.h"

#include <algorithm>
#include <string>

#include "eckit/config/LocalConfiguration.h"

#include "oops/base/Variables.h"
//#include "oops/generic/UnstructuredGrid.h"
#include "model/ModelAtLocations/Gom.h"
#include "model/Locations/Loc.h"
#include "model/ModelBias.h"
#include "model/Fields/Fields.h"
#include "model/Geometry/Geometry.h"
#include "model/Increment/Increment.h"
#include "model/Model/Model.h"
#include "util/DateTime.h"
#include "util/Duration.h"
#include "util/Logger.h"

using oops::Log;

namespace soca {

  // -----------------------------------------------------------------------------
  /// Constructor, destructor
  // -----------------------------------------------------------------------------
  State::State(const Geometry & resol, const oops::Variables & vars,
	       const util::DateTime & vt)
    : fields_(new Fields(resol, vars, vt)), stash_()
  {
    Log::trace() << "State::State created." << std::endl;
  }
  // -----------------------------------------------------------------------------
  State::State(const Geometry & resol, const eckit::Configuration & file)
    : fields_(), stash_()
  {
    const std::vector<std::string> vv{"cicen",
	  "hicen",
	  "hsnon",
	  "tsfcn",
	  "qsnon",
	  "sicnk",
	  "qicnk",
	  "socn",
	  "tocn",
	  "ssh"
	};
    oops::Variables vars(vv);
    fields_.reset(new Fields(resol, vars, util::DateTime()));
    fields_->read(file);
    ASSERT(fields_);
    Log::trace() << "State::State created and read in." << std::endl;
  }
  // -----------------------------------------------------------------------------
  State::State(const Geometry & resol, const State & other)
    : fields_(new Fields(*other.fields_, resol)), stash_()
  {
    ASSERT(fields_);
    Log::trace() << "State::State created by interpolation." << std::endl;
  }
  // -----------------------------------------------------------------------------
  State::State(const State & other)
    : fields_(new Fields(*other.fields_)), stash_()
  {
    ASSERT(fields_);
    Log::trace() << "State::State copied." << std::endl;
  }
  // -----------------------------------------------------------------------------
  State::~State() {
    Log::trace() << "State::State destructed." << std::endl;
  }
  // -----------------------------------------------------------------------------
  void State::activateModel() {
    // Should get variables from model. YT
    //eckit::LocalConfiguration modelvars;
    //modelvars.set("variables", "nl");
    //oops::Variables vars(modelvars);
    // Should get variables from model. YT
    //stash_.reset(new Fields(*fields_, vars));
    //swap(fields_, stash_);
    //ASSERT(fields_);
    //ASSERT(stash_);
    Log::trace() << "State activated for Model" << std::endl;
  }
  // -----------------------------------------------------------------------------
  void State::deactivateModel() {
    //swap(fields_, stash_);
    //*fields_ = *stash_;
    //stash_.reset();
    //ASSERT(fields_);
    //ASSERT(!stash_);
    Log::trace() << "State deactivated for Model" << std::endl;
  }
  // -----------------------------------------------------------------------------
  /// Basic operators
  // -----------------------------------------------------------------------------
  State & State::operator=(const State & rhs) {
    ASSERT(fields_);
    *fields_ = *rhs.fields_;
    return *this;
  }
  // -----------------------------------------------------------------------------
  /// Interpolate to observation location
  // -----------------------------------------------------------------------------
  void State::interpolate(const Loc & locs, const oops::Variables & vars, Gom & cols) const {
    fields_->interpolate(locs, vars, cols);
  }
  // -----------------------------------------------------------------------------
  /// Interpolate full fields
  // -----------------------------------------------------------------------------
  //void State::changeResolution(const State & other) {
  //  fields_->changeResolution(*other.fields_);
  //  Log::trace() << "State interpolated" << std::endl;
  //}
  // -----------------------------------------------------------------------------
  /// Interactions with Increments
  // -----------------------------------------------------------------------------
  State & State::operator+=(const Increment & dx) {
    ASSERT(this->validTime() == dx.validTime());
    ASSERT(fields_);
    fields_->add(dx.fields());
    return *this;
  }
  // -----------------------------------------------------------------------------
  /// Convert to/from unstructured grid
  // -----------------------------------------------------------------------------
  void State::convert_to(oops::UnstructuredGrid & ug) const {
    fields_->convert_to(ug);
  }
  // -----------------------------------------------------------------------------
  void State::convert_from(const oops::UnstructuredGrid & ug) {
    fields_->convert_from(ug);
  }  
  // -----------------------------------------------------------------------------
  /// I/O and diagnostics
  // -----------------------------------------------------------------------------
  void State::read(const eckit::Configuration & files) {
    fields_->read(files);
  }
  // -----------------------------------------------------------------------------
  void State::write(const eckit::Configuration & files) const {
    fields_->write(files);
  }
  // -----------------------------------------------------------------------------
  void State::print(std::ostream & os) const {
    os << std::endl << "  Valid time: " << validTime();
    os << *fields_;
  }
  // -----------------------------------------------------------------------------
  /// For accumulator
  // -----------------------------------------------------------------------------
  void State::zero() {
    fields_->zero();
  }
  // -----------------------------------------------------------------------------
  void State::accumul(const double & zz, const State & xx) {
    fields_->axpy(zz, *xx.fields_);
  }
  // -----------------------------------------------------------------------------

}  // namespace soca
