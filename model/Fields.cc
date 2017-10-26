
#include "model/Fields.h"

#include <cmath>
#include <map>
#include <string>
#include <vector>

#include "util/Logger.h"
#include "model/Fortran.h"
#include "model/Geometry.h"
#include "model/Variables.h"
#include "eckit/config/Configuration.h"
#include "util/DateTime.h"
#include "model/Gom.h"
#include "model/Loc.h"
#include "oops/generic/UnstructuredGrid.h"
#include "util/Logger.h"


// -----------------------------------------------------------------------------
namespace mom5cice5 {
  // -----------------------------------------------------------------------------
  Fields::Fields(const Geometry & geom, const Variables & vars,
		 const util::DateTime & time):
    geom_(new Geometry(geom)), vars_(new Variables(vars)), time_(time)
  {
    mom5cice5_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
  }
  // -----------------------------------------------------------------------------
  Fields::Fields(const Fields & other, const bool copy)
    : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
  {
    mom5cice5_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
    if (copy) {
      mom5cice5_field_copy_f90(keyFlds_, other.keyFlds_);
    } else {
      mom5cice5_field_zero_f90(keyFlds_);
    }
  }
  // -----------------------------------------------------------------------------
  Fields::Fields(const Fields & other)
    : geom_(other.geom_), vars_(other.vars_), time_(other.time_)
  {
    mom5cice5_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
    mom5cice5_field_copy_f90(keyFlds_, other.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  Fields::Fields(const Fields & other, const Geometry & geom)
    : geom_(new Geometry(geom)), vars_(other.vars_), time_(other.time_)
  {
    mom5cice5_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
    mom5cice5_field_change_resol_f90(keyFlds_, other.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  Fields::Fields(const Fields & other, const Variables & vars)
    : geom_(other.geom_), vars_(new Variables(vars)), time_(other.time_)
  {
    mom5cice5_field_create_f90(keyFlds_, geom_->toFortran(), vars_->toFortran());
    mom5cice5_field_copy_f90(keyFlds_, other.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  Fields::~Fields() {
    mom5cice5_field_delete_f90(keyFlds_);
  }
  // -----------------------------------------------------------------------------
  Fields & Fields::operator=(const Fields & rhs) {
    mom5cice5_field_copy_f90(keyFlds_, rhs.keyFlds_);
    time_ = rhs.time_;
    return *this;
  }
  // -----------------------------------------------------------------------------
  Fields & Fields::operator+=(const Fields & rhs) {
    mom5cice5_field_self_add_f90(keyFlds_, rhs.keyFlds_);
    return *this;
  }
  // -----------------------------------------------------------------------------
  Fields & Fields::operator-=(const Fields & rhs) {
    mom5cice5_field_self_sub_f90(keyFlds_, rhs.keyFlds_);
    return *this;
  }
  // -----------------------------------------------------------------------------
  Fields & Fields::operator*=(const double & zz) {
    mom5cice5_field_self_mul_f90(keyFlds_, zz);
    return *this;
  }
  // -----------------------------------------------------------------------------
  void Fields::zero() {
    mom5cice5_field_zero_f90(keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::zero(const util::DateTime & time) {
    mom5cice5_field_zero_f90(keyFlds_);
    time_ = time;
  }
  // -----------------------------------------------------------------------------
  void Fields::dirac(const eckit::Configuration & config) {
    const eckit::Configuration * conf = &config;
    mom5cice5_field_dirac_f90(keyFlds_, &conf);
  }
  // -----------------------------------------------------------------------------
  void Fields::axpy(const double & zz, const Fields & rhs) {
    mom5cice5_field_axpy_f90(keyFlds_, zz, rhs.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  double Fields::dot_product_with(const Fields & fld2) const {
    double zz;
    mom5cice5_field_dot_prod_f90(keyFlds_, fld2.keyFlds_, zz);
    return zz;
  }
  // -----------------------------------------------------------------------------
  void Fields::schur_product_with(const Fields & dx) {
    mom5cice5_field_self_schur_f90(keyFlds_, dx.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::random() {
    mom5cice5_field_random_f90(keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::interpolate(const Loc & locs, Gom & gom) const {
    mom5cice5_field_interp_tl_f90(keyFlds_, locs.toFortran(), gom.toFortran());
  }
  // -----------------------------------------------------------------------------
  void Fields::interpolateTL(const Loc & locs, Gom & gom) const {
    mom5cice5_field_interp_tl_f90(keyFlds_, locs.toFortran(), gom.toFortran());
  }
  // -----------------------------------------------------------------------------
  void Fields::interpolateAD(const Loc & locs, const Gom & gom) {
    mom5cice5_field_interp_ad_f90(keyFlds_, locs.toFortran(), gom.toFortran());
  }
  // -----------------------------------------------------------------------------
  void Fields::changeResolution(const Fields & other) {
    mom5cice5_field_change_resol_f90(keyFlds_, other.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::add(const Fields & rhs) {
    mom5cice5_field_add_incr_f90(keyFlds_, rhs.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::diff(const Fields & x1, const Fields & x2) {
    mom5cice5_field_diff_incr_f90(keyFlds_, x1.keyFlds_, x2.keyFlds_);
  }
  // -----------------------------------------------------------------------------
  void Fields::convert_to(oops::UnstructuredGrid & ug) const {
    mom5cice5_field_convert_to_f90(keyFlds_, ug.toFortran());
  }
  // -----------------------------------------------------------------------------
  void Fields::convert_from(const oops::UnstructuredGrid & ug) {
    mom5cice5_field_convert_from_f90(keyFlds_, ug.toFortran());
  }
  // -----------------------------------------------------------------------------
  void Fields::read(const eckit::Configuration & config) {
    const eckit::Configuration * conf = &config;
    util::DateTime * dtp = &time_;
    mom5cice5_field_read_file_f90(keyFlds_, &conf, &dtp);
  }
  // -----------------------------------------------------------------------------
  void Fields::write(const eckit::Configuration & config) const {
    const eckit::Configuration * conf = &config;
    const util::DateTime * dtp = &time_;
    mom5cice5_field_write_file_f90(keyFlds_, &conf, &dtp);
  }
  // -----------------------------------------------------------------------------
  double Fields::norm() const {
    double zz = 0.0;
    mom5cice5_field_rms_f90(keyFlds_, zz);
    return zz;
  }
  // -----------------------------------------------------------------------------
  void Fields::print(std::ostream & os) const {
    int nx = -1;
    int ny = -1;
    int nzo = -1;
    int nzi = -1;
    int ncat = -1;
    int nf = -1;
    mom5cice5_field_sizes_f90(keyFlds_, nx, ny, nzo, nzi, ncat, nf);
    os << std::endl << "  Resolution = " << nx << ", " << ny
       << ", Fields = " << nf;
    std::vector<double> zstat(3*nf);
    mom5cice5_field_gpnorm_f90(keyFlds_, nf, zstat[0]);
    for (int jj = 0; jj < nf; ++jj) {
      os << std::endl << "  Min=" << zstat[3*jj]
	 << ", Max=" << zstat[3*jj+1] << ", RMS=" << zstat[3*jj+2];
    }
  }
  // -----------------------------------------------------------------------------
  bool Fields::isForModel(bool nonlinear) const {
    int nx = -1;
    int ny = -1;
    int nzo = -1;
    int nzi = -1;
    int ncat = -1;
    int nf = -1;
    mom5cice5_field_sizes_f90(keyFlds_, nx, ny, nzo, nzi, ncat, nf);
  
    //int nx = -1;
    //int ny = -1;
    //int nf = -1;
    //int nb = -1;
    //mom5cice5_field_sizes_f90(keyFlds_, nx, ny, nf, nb);
    bool ok = (nf == 12);   //<---- HARD CODED STUFF ... NEED TO CHANGE
    if (nonlinear) ok = ok; // && (nb == 2);
    return ok;
  }
  // -----------------------------------------------------------------------------
}  // namespace mom5cice5
