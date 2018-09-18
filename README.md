
<!-- README.md is generated from README.Rmd. Please edit that file -->

# altrepisode

``` r
library(altrepisode)

x <- stdvec_double()
stdvec_double_push_back(x, 2)
stdvec_double_push_back(x, 3)

x
#> [1] 2 3
.Internal(inspect(x))
#> @7fa8785282e0 14 REALSXP g0c0 [NAM(3)] (len=2, tl=0) 2,3
```
