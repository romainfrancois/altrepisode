#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

#define class klass
extern "C" {
  #include <R_ext/Altrep.h>
}
#undef class
