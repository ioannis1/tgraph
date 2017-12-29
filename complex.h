#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"             /* needed for send/recv functions */
#include <math.h>
#include "utils/guc.h"


#ifndef _COMPLEX_NUMBER
#define _COMPLEX_NUMBER

PG_MODULE_MAGIC;

typedef struct Complex {
    double      x;
    double      y;
} Complex;


#define MAG_SQUARED(c)      ((c)->x*(c)->x + (c)->y*(c)->y)
#define SENSITIVITY         1e-8

PG_FUNCTION_INFO_V1(complex_in);
PG_FUNCTION_INFO_V1(complex_out);
PG_FUNCTION_INFO_V1(complex_recv);
PG_FUNCTION_INFO_V1(complex_send);
PG_FUNCTION_INFO_V1(complex_conjugate);
PG_FUNCTION_INFO_V1(complex_add);
PG_FUNCTION_INFO_V1(complex_mult);
PG_FUNCTION_INFO_V1(complex_subtract);
PG_FUNCTION_INFO_V1(complex_theta);
PG_FUNCTION_INFO_V1(complex_xy);
PG_FUNCTION_INFO_V1(complex_polar);
PG_FUNCTION_INFO_V1(complex_theta_add);
PG_FUNCTION_INFO_V1(complex_mag);
PG_FUNCTION_INFO_V1(complex_mag_squared);
PG_FUNCTION_INFO_V1(complex_abs_cmp);
PG_FUNCTION_INFO_V1(complex_equal);
PG_FUNCTION_INFO_V1(complex_less);
PG_FUNCTION_INFO_V1(complex_less_equal);
PG_FUNCTION_INFO_V1(complex_greater_equal);
PG_FUNCTION_INFO_V1(complex_greater);
PG_FUNCTION_INFO_V1(complex_overlaps);
PG_FUNCTION_INFO_V1(complex_real);
PG_FUNCTION_INFO_V1(complex_img);
PG_FUNCTION_INFO_V1(complex_new_polar);
PG_FUNCTION_INFO_V1(complex_new);
PG_FUNCTION_INFO_V1(complex_int2c_cmp);
PG_FUNCTION_INFO_V1(complex_int2c_equal);
PG_FUNCTION_INFO_V1(complex_int2c_less);
PG_FUNCTION_INFO_V1(complex_int2c_less_equal);
PG_FUNCTION_INFO_V1(complex_int2c_greater_equal);
PG_FUNCTION_INFO_V1(complex_int2c_greater);

#endif
