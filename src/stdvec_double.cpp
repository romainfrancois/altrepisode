#include "altrepisode.h"

// We need the std::vector template class, so that
// we can use std::vector<double>
#include <vector>

// instead of defining a set of free functions, we structure them
// together in a struct
struct stdvec_double {

  // The altrep class that wraps a pointer to a std::vector<double>
  static R_altrep_class_t class_t;

  // Make an altrep object of class `stdvec_double::class_t`
  static SEXP Make(std::vector<double>* data, bool owner){
    // The std::vector<double> pointer is wrapped into an R external pointer
    //
    // `xp` needs protection because R_new_altrep allocates
    SEXP xp = PROTECT(R_MakeExternalPtr(data, R_NilValue, R_NilValue));

    // If we own the std::vector<double>*, we need to delete it
    // when the R object is being garbage collected
    if (owner) {
      R_RegisterCFinalizerEx(xp, stdvec_double::Finalize, TRUE);
    }

    // make a new altrep object of class `stdvec_double::class_t`
    SEXP res = R_new_altrep(class_t, xp, R_NilValue);

    // xp no longer needs protection, as it has been adopted by `res`
    UNPROTECT(1);
    return res;
  }

  // finalizer for the external pointer
  static void Finalize(SEXP xp){
    delete static_cast<std::vector<double>*>(R_ExternalPtrAddr(xp));
  }

  // get the std::vector<double>* from the altrep object `x`
  static std::vector<double>* Ptr(SEXP x) {
    return static_cast<std::vector<double>*>(R_ExternalPtrAddr(R_altrep_data1(x)));
  }

  // same, but as a reference, for convenience
  static std::vector<double>& Get(SEXP vec) {
    return *Ptr(vec) ;
  }

  // ALTREP methods -------------------

  // The length of the object
  static R_xlen_t Length(SEXP vec){
    return Get(vec).size();
  }

  // What gets printed when .Internal(inspect()) is used
  static Rboolean Inspect(SEXP x, int pre, int deep, int pvec, void (*inspect_subtree)(SEXP, int, int, int)){
    Rprintf("std::vector<double> (len=%d, ptr=%p)\n", Length(x), Ptr(x));
    return TRUE;
  }

  // ALTVEC methods ------------------

  // The start of the data, i.e. the underlying double* array from the std::vector<double>
  //
  // This is guaranteed to never allocate (in the R sense)
  static const void* Dataptr_or_null(SEXP vec){
    return Get(vec).data();
  }

  // same in this case, writeable is ignored
  static void* Dataptr(SEXP vec, Rboolean writeable){
    return Get(vec).data();
  }


  // ALTREAL methods -----------------

  // the element at the index `i`
  //
  // this does not do bounds checking because that's expensive, so
  // the caller must take care of that
  static double real_Elt(SEXP vec, R_xlen_t i){
    return Get(vec)[i];
  }

  // Get a pointer to the region of the data starting at index `i`
  // of at most `size` elements.
  //
  // The return values is the number of elements the region truly is so the caller
  // must not go beyond
  static R_xlen_t Get_region(SEXP vec, R_xlen_t start, R_xlen_t size, double* out){
    out = Get(vec).data() + start;
    R_xlen_t len = Get(vec).size() - start;
    return len > size ? len : size;
  }

  // -------- initialize the altrep class with the methods above

  static void Init(DllInfo* dll){
    class_t = R_make_altreal_class("stdvec_double", "altrepisode", dll);

    // altrep
    R_set_altrep_Length_method(class_t, Length);
    R_set_altrep_Inspect_method(class_t, Inspect);

    // altvec
    R_set_altvec_Dataptr_method(class_t, Dataptr);
    R_set_altvec_Dataptr_or_null_method(class_t, Dataptr_or_null);

    // altreal
    R_set_altreal_Elt_method(class_t, real_Elt);
    R_set_altreal_Get_region_method(class_t, Get_region);
  }

};

// static initialization of stdvec_double::class_t
R_altrep_class_t stdvec_double::class_t;

// Called the package is loaded (needs Rcpp 0.12.18.3)
// [[Rcpp::init]]
void init_stdvec_double(DllInfo* dll){
  stdvec_double::Init(dll);
}

//' an altrep object that wraps a std::vector<double>
//'
//' @export
// [[Rcpp::export]]
SEXP doubles() {
  // create a new std::vector<double>
  //
  // this uses `new` because we want the vector to survive
  // it is deleted when the altrep object is garbage collected
  auto v = new std::vector<double> {-2.0, -1.0, 0.0, 1.0, 2.0};

  // The altrep object owns the std::vector<double>
  return stdvec_double::Make(v, true);
}

// example C++ function that returns `n` random number between 0 and 1
std::vector<double> randoms(int n){
  std::vector<double> v(n);
  std::generate(begin(v), end(v), [] { return (double)std::rand() / RAND_MAX ; } );
  return v;
}

// [[Rcpp::export]]
SEXP doubles_example(){
  // get a std::vector<double> from somewhere
  auto v = randoms(10);

  // wrap it into an altrep object of class `stdvec_double::class_t`
  // the altrep object does not own the vector, because we only need
  // it within this scope, it will be deleted just like any C++ object
  // at the end of this C++ function
  SEXP x = PROTECT(stdvec_double::Make(&v, false));

  // call sum(x) in the base environment
  SEXP s_sum = Rf_install("sum");
  SEXP call = PROTECT(Rf_lang2(s_sum, x));
  SEXP res = Rf_eval(call, R_BaseEnv);

  UNPROTECT(2);

  return res;
}
