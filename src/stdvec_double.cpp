#include "altrepisode.h"
#include <vector>

struct stdvec_double {
  static R_altrep_class_t class_t;

  static std::vector<double>* Ptr(SEXP vec) {
    return reinterpret_cast<std::vector<double>*>(R_ExternalPtrAddr(R_altrep_data1(vec)));
  }

  static std::vector<double>& Get(SEXP vec) {
    return *Ptr(vec) ;
  }

  static SEXP Make(std::vector<double>* data, bool owner){
    SEXP xp = PROTECT(R_MakeExternalPtr(data, R_NilValue, R_NilValue));
    if (owner) {
      R_RegisterCFinalizerEx(xp, stdvec_double::Finalize, TRUE);
    }
    UNPROTECT(1);
    return ::R_new_altrep(class_t, xp, R_NilValue);
  }

  static void Finalize(SEXP xp){
    delete reinterpret_cast<std::vector<double>*>(R_ExternalPtrAddr(xp));
  }

  // altrep
  static R_xlen_t Length(SEXP vec){
    return Get(vec).size();
  }

  static Rboolean Inspect(SEXP x, int pre, int deep, int pvec, void (*inspect_subtree)(SEXP, int, int, int)){
    Rprintf("\033[31mstd::vector<double> (len=%d, ptr=%p)\033[39m\n", Length(x), Ptr(x) );
    return TRUE;
  }


  static void* Dataptr(SEXP vec, Rboolean writeable){
    return Get(vec).data();
  }

  static const void* Dataptr_or_null(SEXP vec){
    return Get(vec).data();
  }

  static double real_Elt(SEXP vec, R_xlen_t i){
    return Get(vec)[i];
  }

  static R_xlen_t Get_region(SEXP vec, R_xlen_t start, R_xlen_t size, double* out){
    out = Get(vec).data() + start;
    R_xlen_t len = Get(vec).size() - start;
    return len > size ? len : size;
  }

  static void Init(DllInfo* dll){
    class_t = R_make_altreal_class("stdvec_double", "altrepisode", dll);

    // altrep
    R_set_altrep_Length_method(class_t, stdvec_double::Length);
    R_set_altrep_Inspect_method(class_t, stdvec_double::Inspect);

    // altvec
    R_set_altvec_Dataptr_method(class_t, stdvec_double::Dataptr);
    R_set_altvec_Dataptr_or_null_method(class_t, stdvec_double::Dataptr_or_null);

    // altreal
    R_set_altreal_Elt_method(class_t, stdvec_double::real_Elt);
    R_set_altreal_Get_region_method(class_t, stdvec_double::Get_region);
  }

};

R_altrep_class_t stdvec_double::class_t;

// [[Rcpp::init]]
void init_altrep_classes(DllInfo* dll){
  stdvec_double::Init(dll);
}

//' @export
// [[Rcpp::export]]
SEXP doubles() {
  auto v = new std::vector<double> {0.0, 1.0, 2.0, 3.0};
  return stdvec_double::Make(v, true);
}


std::vector<double> randoms(int n){
  std::vector<double> v(n);
  std::generate(begin(v), end(v), [] { return (double)std::rand() / RAND_MAX ; } );
  return v;
}

//' @export
// [[Rcpp::export]]
SEXP doubles_example(){
  auto v = randoms(10);
  SEXP x = PROTECT(stdvec_double::Make(&v, false));

  // call R sum function
  SEXP s_sum = Rf_install("sum");
  SEXP call = PROTECT(Rf_lang2(s_sum, x));
  SEXP res = PROTECT(Rf_eval(call, R_BaseEnv));
  UNPROTECT(3);

  return res;
}



