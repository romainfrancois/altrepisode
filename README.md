
<!-- README.md is generated from README.Rmd. Please edit that file -->

# altrepisode

``` r
library(altrepisode)

x <- doubles()
x[2]
#> [1] -1
.Internal(inspect(x))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97adb80e0 14 REALSXP g0c0 [NAM(3)] <span style='color: #BB0000;'>std::vector&lt;double&gt; (len=5, ptr=0x7ff973ea66a0)</span><span>
</span></CODE></PRE>

``` r

set.seed(2)
y <- rnorm(20)

# lazy abs version of y
abs_y <- lazy_abs(y)
.Internal(inspect(abs_y))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97b009c08 14 REALSXP g0c0 [NAM(3)] <span style='color: #BB0000;'>lazy(abs) (len=0)</span><span>
#&gt;   @7ff973e8f3a0 14 REALSXP g0c7 [NAM(3)] (len=20, tl=0) -0.896915,0.184849,1.58785,-1.13038,-0.0802518,...
</span></CODE></PRE>

``` r

# grab a single value
# that does not materialize the data
abs_y[1]
#> [1] 0.8969145
.Internal(inspect(abs_y))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97b009c08 14 REALSXP g0c0 [NAM(3)] <span style='color: #BB0000;'>lazy(abs) (len=0)</span><span>
#&gt;   @7ff973e8f3a0 14 REALSXP g0c7 [NAM(3)] (len=20, tl=0) -0.896915,0.184849,1.58785,-1.13038,-0.0802518,...
</span></CODE></PRE>

``` r

# but mean does
mean(abs_y)
#> [1] 0.8184071
.Internal(inspect(abs_y))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97b009c08 14 REALSXP g0c0 [NAM(3)] <span style='color: #00BB00;'>materialized lazy(abs) (len=0)</span><span>
#&gt;   @7ff973e8f3a0 14 REALSXP g0c7 [NAM(3)] (len=20, tl=0) -0.896915,0.184849,1.58785,-1.13038,-0.0802518,...
#&gt;   @7ff973e73d10 14 REALSXP g0c7 [] (len=20, tl=0) 0.896915,0.184849,1.58785,1.13038,0.0802518,...
</span></CODE></PRE>

``` r


# we can combine both
abs_x <- lazy_abs(x)
.Internal(inspect(abs_x))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97b2d5678 14 REALSXP g0c0 [NAM(3)] <span style='color: #BB0000;'>lazy(abs) (len=0)</span><span>
#&gt;   @7ff97adb80e0 14 REALSXP g0c0 [NAM(3)] </span><span style='color: #BB0000;'>std::vector&lt;double&gt; (len=5, ptr=0x7ff973ea66a0)</span><span>
</span></CODE></PRE>

``` r
mean(abs_x)
#> [1] 0
.Internal(inspect(abs_x))
```

<PRE class="fansi fansi-output"><CODE>#&gt; @7ff97b2d5678 14 REALSXP g0c0 [NAM(3)] <span style='color: #00BB00;'>materialized lazy(abs) (len=0)</span><span>
#&gt;   @7ff97adb80e0 14 REALSXP g0c0 [NAM(3)] </span><span style='color: #BB0000;'>std::vector&lt;double&gt; (len=5, ptr=0x7ff973ea66a0)</span><span>
#&gt;   @7ff97b3112e8 14 REALSXP g0c4 [] (len=5, tl=0) 2,1,0,1,2
</span></CODE></PRE>
