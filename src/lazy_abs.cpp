#include "altrepisode.h"

#include <algorithm>



struct lazy_abs {
  static R_altrep_class_t class_t;

  // make a new altrep object of class lazy_abs::class_t
  //
  // the object uses both altrep data
  // - data1: the original numeric vector we want to abs lazily
  // - data2: NULL if the object has not been materialized yet, the entire abs(data1) otherwise
  static SEXP Make(SEXP data){
    // so that other holders of data know they have to make a copy
    // when they mutate it
    MARK_NOT_MUTABLE(data);

    // data2 is originally set to NULL
    return R_new_altrep(class_t, data, R_NilValue);
  }

  // materialize data2 if needed and return it
  static SEXP Materialize(SEXP vec) {

    SEXP data2 = R_altrep_data2(vec);
    if (data2 == R_NilValue) {
      // so we do have to materialize
      SEXP data1 = R_altrep_data1(vec);

      // allocate a standard numeric vector for data2
      R_xlen_t n = LENGTH(data1);
      data2 = PROTECT(Rf_allocVector(REALSXP, n));
      // we know it is a standard vector, so it's
      // fine to get its pointer
      double* p_data2 = REAL0(data2);

      // we need to treat the data differently depending
      // on its altrep properties
      auto p_data1 = DATAPTR_OR_NULL(data1);
      if (p_data1){
        // the data from data1 is contiguous, we can scan the values
        // from data1, apply the fabs function and fill data2
        auto p = static_cast<const double*>(p_data1);
        std::transform(
          p, p + n, // the input range
          p_data2,  // the output start
          [](double x){ return fabs(x) ; } // We can't just use fabs because there are overloads.
        );
      } else {
        // the data is not contiguous, so we avoid materializing it
        // and rather go through it element by element
        for (R_xlen_t i=0; i<n; i++, ++p_data2) {
          *p_data2 = ::fabs(ALTREAL_ELT(data1, i)) ;
        }
      }

      // keep data2 and release its protection
      R_set_altrep_data2(vec, data2);
      UNPROTECT(1);
    }
    return data2;
  }

  // ALTREP methods -------------------

  // The length of the object, that's just the length of data1
  static R_xlen_t Length(SEXP vec){
    return LENGTH(R_altrep_data1(vec));
  }

  // What gets printed when .Internal(inspect()) is used
  static Rboolean Inspect(SEXP x, int pre, int deep, int pvec, void (*inspect_subtree)(SEXP, int, int, int)){
    SEXP data1 = R_altrep_data1(x);
    SEXP data2 = R_altrep_data2(x);

    if (data2 == R_NilValue) {
      Rprintf("\033[31mlazy(abs) (len=%d)\033[39m\n");
      inspect_subtree(data1, pre, deep + 1, pvec);
    } else {
      Rprintf("\033[32mmaterialized lazy(abs) (len=%d)\033[39m\n");
      inspect_subtree(data1, pre, deep + 1, pvec);
      inspect_subtree(data2, pre, deep + 1, pvec);
    }
    return TRUE;
  }

  // ALTVEC methods ------------------

  // If the data has already been materialized into contiguous, then return its
  // dataptr, otherwise return a null pointer
  static const void* Dataptr_or_null(SEXP vec){
    SEXP data2 = R_altrep_data2(vec);
    if (data2 == R_NilValue) return nullptr;

    return DATAPTR(data2);
  }

  // if the data has already been materialized, return that,
  // otherwise, materialize it into the `data2` part of the altrep object
  static void* Dataptr(SEXP vec, Rboolean writeable){
    return STDVEC_DATAPTR(Materialize(vec));
  }

  // ALTREAL methods -----------------

  // the element at the index `i`
  //
  // if the data has been materialized, get the value from data2,
  // otherwise calculate just that value from data1
  static double real_Elt(SEXP vec, R_xlen_t i){
    SEXP data2 = R_altrep_data2(vec);
    if (data2 == R_NilValue) {
      SEXP data1 = R_altrep_data1(vec);
      return ::fabs(REAL_ELT(data1, i));
    } else {
      return static_cast<double*>(STDVEC_DATAPTR(data2))[i];
    }
  }

  // Get a pointer to the region of the data starting at index `i`
  // of at most `size` elements.
  //
  // The return values is the number of elements the region truly is so the caller
  // must not go beyond
  static R_xlen_t Get_region(SEXP vec, R_xlen_t start, R_xlen_t size, double* out){
    double* data = static_cast<double*>(Dataptr(vec, TRUE));
    out = data + start;
    R_xlen_t len = Length(vec) - start;
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
R_altrep_class_t lazy_abs::class_t;

// Called the package is loaded (needs Rcpp 0.12.18.3)
// [[Rcpp::init]]
void init_lazy_abs_class(DllInfo* dll){
  lazy_abs::Init(dll);
}

//' lazy altrep version of abs
//'
//' @export
// [[Rcpp::export]]
SEXP lazy_abs(SEXP x) {
  return lazy_abs::Make(x);
}


