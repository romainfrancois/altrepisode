#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#define class klass
extern "C" {
  #include <R_ext/Altrep.h>
}
#undef class

#include <vector>

static R_altrep_class_t altrep_stdvec_double_class;

class stdvec_double {
public:
  std::vector<double> data;

  static stdvec_double* Get(SEXP vec){
    return reinterpret_cast<stdvec_double*>(R_ExternalPtrAddr(R_altrep_data1(vec))) ;
  }

  static SEXP Make(){
    stdvec_double* vec = new stdvec_double;

    SEXP xp = PROTECT(R_MakeExternalPtr(vec, R_NilValue, R_NilValue));
    R_RegisterCFinalizerEx(xp, stdvec_double::Finalize, TRUE);
    UNPROTECT(1);
    return ::R_new_altrep(altrep_stdvec_double_class, xp, R_NilValue);
  }

  static void Finalize(SEXP xp){
    stdvec_double* p = reinterpret_cast<stdvec_double*>(R_ExternalPtrAddr(xp));
    if (p) {
      delete p;
    }
  }

  static R_xlen_t Length(SEXP vec){
    return Get(vec)->data.size();
  }

  static void* Dataptr(SEXP vec, Rboolean writeable){
    return Get(vec)->data.data();
  }

  static const void* Dataptr_or_null(SEXP vec){
    return Get(vec)->data.data();
  }

  static double real_Elt(SEXP vec, R_xlen_t i){
    return Get(vec)->data[i];
  }

  static R_xlen_t Get_region(SEXP vec, R_xlen_t start, R_xlen_t size, double* out){
    out = Get(vec)->data.data() + start;
    return size;
  }

};

// [[Rcpp::init]]
void init_altrep_classes(DllInfo* dll){
  altrep_stdvec_double_class = R_make_altreal_class("stdvec_double", "altrepisode", dll);

  R_set_altrep_Length_method(altrep_stdvec_double_class, stdvec_double::Length);

  R_set_altvec_Dataptr_method(altrep_stdvec_double_class, stdvec_double::Dataptr);
  R_set_altvec_Dataptr_or_null_method(altrep_stdvec_double_class, stdvec_double::Dataptr_or_null);

  R_set_altreal_Elt_method(altrep_stdvec_double_class, stdvec_double::real_Elt);
  R_set_altreal_Get_region_method(altrep_stdvec_double_class, stdvec_double::Get_region);
}

//' @export
// [[Rcpp::export]]
SEXP stdvec_double(){
  return stdvec_double::Make();
}

//' @export
// [[Rcpp::export]]
void stdvec_double_reserve(SEXP vec, R_xlen_t size){
  stdvec_double::Get(vec)->data.reserve(size);
}

//' @export
// [[Rcpp::export]]
void stdvec_double_push_back(SEXP vec, double x){
  stdvec_double::Get(vec)->data.push_back(x);
}



