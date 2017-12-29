#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "libpq/pqformat.h"             /* needed for send/recv functions */
#include <math.h>
#include "utils/guc.h"


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


Datum
complex_in(PG_FUNCTION_ARGS)
{
    char       *str = PG_GETARG_CSTRING(0);
    double     x, y;
    Complex    *result;
    const char  *output_style = GetConfigOption("complex.style",true,false);


    if (sscanf(str, " ( %lf , %lf  )", &x, &y) != 2)
         ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                       errmsg("invalid: expected syntax is (r,angle): \"%s\"", str)));

     result = (Complex *) palloc(sizeof(Complex));


     //TODO: IF is never executed
     if ( (NULL != output_style) && !(strncmp( output_style, "polar",5)) ) {
             x      =  x * ( cos(y * (3.14159/180) )) ;
             y      =  x * ( sin(y * (3.14159/180) )) ;
             if (x< SENSITIVITY) x = 0;
             if (y< SENSITIVITY) y = 0;
             result->x = x;
             result->y = y;
             PG_RETURN_POINTER(result);
     }
     result->x = x;
     result->y = y;
     PG_RETURN_POINTER(result);
}


Datum
complex_out(PG_FUNCTION_ARGS)
{
    Complex    *a = (Complex *) PG_GETARG_POINTER(0);
    char       *result;
    float4      mag, angle;
    const char  *output_style = GetConfigOption("complex.style",true,false);

     if ( (NULL != output_style) && !(strncmp( output_style, "polar",5)) ) {
            mag    =  sqrt( MAG_SQUARED(a) );
            angle  =  (180/3.14159)* atan(a->y/a->x);
            result = psprintf("%g<%g>", mag, angle);
     }else{
            result = psprintf("(%g,%g)", a->x, a->y);
     }
     PG_RETURN_CSTRING(result);
}

Datum
complex_recv(PG_FUNCTION_ARGS)
{
    StringInfo  buf = (StringInfo) PG_GETARG_POINTER(0);
    Complex    *result;

    result = (Complex *) palloc(sizeof(Complex));
    result->x = pq_getmsgfloat8(buf);
    result->y = pq_getmsgfloat8(buf);
    PG_RETURN_POINTER(result);
}


Datum
complex_send(PG_FUNCTION_ARGS)
{
    Complex    *complex = (Complex *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendfloat8(&buf, complex->x);
    pq_sendfloat8(&buf, complex->y);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

Datum
complex_conjugate(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *result;

        result = (Complex *) palloc(sizeof(Complex));
        result->x = a->x;
        result->y = -1*(a->y);
        PG_RETURN_POINTER(result);
}


Datum
complex_add(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        Complex    *result;

        result = (Complex *) palloc(sizeof(Complex));
        result->x = a->x + b->x;
        result->y = a->y + b->y;
        PG_RETURN_POINTER(result);
}


Datum
complex_mult(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        Complex    *result;

        result = (Complex *) palloc(sizeof(Complex));
        result->x = (a->x * b->x) - (a->y * b->y);
        result->y = (a->x * b->y) + (a->y*b->x);
        PG_RETURN_POINTER(result);
}




Datum
complex_subtract(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        Complex    *result;

        result = (Complex *) palloc(sizeof(Complex));
        result->x = a->x - b->x;
        result->y = a->y - b->y;
        PG_RETURN_POINTER(result);
}




Datum
complex_theta(PG_FUNCTION_ARGS)
{
    Complex    *a = (Complex *) PG_GETARG_POINTER(0);
    float4    result;

    result =  (180/3.14159)* atan(a->y/a->x);

    PG_RETURN_FLOAT4(result );
}

Datum
complex_xy(PG_FUNCTION_ARGS)
{
     // Convert complex from polar to cartesian cordinates
     // Input: c->x is magnitude
     // Input: c->y is angle
     // Output: c->x is x coordinate
     // Output: c->y is y coordinate
     Complex    *a = (Complex *) PG_GETARG_POINTER(0);
     Complex    *result;
     float4     x,y;

     result = (Complex *) palloc(sizeof(Complex));
     x      =  a->x * ( cos(a->y * (3.14159/180) )) ;
     y      =  a->x * ( sin(a->y * (3.14159/180) )) ;

     if (x< SENSITIVITY) x = 0;
     if (y< SENSITIVITY) y = 0;
     result->x = x;
     result->y = y;
     PG_RETURN_POINTER(result);
}


Datum
complex_polar(PG_FUNCTION_ARGS)
{
     // Convert complex from cartesian to polar coordinates
     // Output: c->x is magnitude
     // Output: c->y is angle
     Complex    *a = (Complex *) PG_GETARG_POINTER(0);
     Complex    *result;
     float4     angle;

     result = (Complex *) palloc(sizeof(Complex));

     float4    mag  = sqrt( MAG_SQUARED(a) );
     angle  =  (180/3.14159)* atan(a->y/a->x);
     result->x = mag;
     result->y =  angle;
     PG_RETURN_POINTER(result);
}


Datum
complex_theta_add(PG_FUNCTION_ARGS)
{
     Complex    *a = (Complex *) PG_GETARG_POINTER(0);
     float4      phi  = PG_GETARG_FLOAT4(1);
     Complex    *result;
     float4     old_angle, new_angle;
     float4    mag ;
    

     result = (Complex *) palloc(sizeof(Complex));

     mag  = sqrt( MAG_SQUARED(a) );
     old_angle  =  (180/3.14159)* atan(a->y/a->x);

     result->x  =  mag * cos( (old_angle+phi) * 3.14159/180);
     result->y  =  mag * sin( (old_angle+phi) * 3.14159/180);
     PG_RETURN_POINTER(result);
}


Datum
complex_mag(PG_FUNCTION_ARGS)
{
    Complex    *a = (Complex *) PG_GETARG_POINTER(0);
    float4    result =  MAG_SQUARED(a);
    result   = sqrt(result);

    PG_RETURN_FLOAT4(result );
}


Datum
complex_mag_squared(PG_FUNCTION_ARGS)
{
    Complex    *a = (Complex *) PG_GETARG_POINTER(0);
    float4    result =  MAG_SQUARED(a);

    PG_RETURN_FLOAT4(result );
}


static int
complex_cmp_internal( Complex *a, Complex *b ) 
{
     double   amag = MAG_SQUARED(a),  bmag=MAG_SQUARED(b);
     if ( amag < bmag  ) return -1;
     if ( amag == bmag ) return 0 ;
     return 1 ;
}

Datum
complex_abs_cmp(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        PG_RETURN_INT32( complex_cmp_internal(a,b) );
}



Datum
complex_equal(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_cmp_internal(a,b) == 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_less(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_cmp_internal(a,b) == -1 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}

Datum
complex_less_equal(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_cmp_internal(a,b) <= 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_greater_equal(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_cmp_internal(a,b) >= 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_greater(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_cmp_internal(a,b) > 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_overlaps(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if ( a->x < b->x)  PG_RETURN_BOOL(false);
        if ( a->y < b->y)  PG_RETURN_BOOL(false);
        PG_RETURN_BOOL(true);
}

Datum
complex_real(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        
        PG_RETURN_FLOAT4(a->x);
}


Datum
complex_img(PG_FUNCTION_ARGS)
{
        Complex    *a = (Complex *) PG_GETARG_POINTER(0);
        
        PG_RETURN_FLOAT4(a->y);
}



Datum
complex_new_polar(PG_FUNCTION_ARGS)
{
    float4     mag   = PG_GETARG_FLOAT4(0);
    float4     theta = PG_GETARG_FLOAT4(1);
    Complex   *result;
    float4     x,y;

    result = (Complex *) palloc(sizeof(Complex));

     x      =  mag * ( cos( theta  * (3.14159/180) )) ;
     y      =  mag * ( sin( theta  * (3.14159/180) )) ;

    if (x< SENSITIVITY) x = 0;
    if (y< SENSITIVITY) y = 0;

    result->x = x;
    result->y = y;
    PG_RETURN_POINTER( result );
}


Datum
complex_new(PG_FUNCTION_ARGS)
{
    float4      a = PG_GETARG_FLOAT4(0);
    float4      b = PG_GETARG_FLOAT4(1);
    Complex   *result;

    result = (Complex *) palloc(sizeof(Complex));
    result->x = a;
    result->y = b;
    PG_RETURN_POINTER(result);
}

// cross-data comparisons


static int
complex_int2c_cmp_internal( int32 a, Complex *b ) 
{
     double     bmag= MAG_SQUARED(b);
     if ( a < bmag  ) return -1;
     if ( a == bmag ) return 0 ;
     return 1 ;
}

Datum
complex_int2c_cmp(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        PG_RETURN_INT32( complex_int2c_cmp_internal(a,b) );
}



Datum
complex_int2c_equal(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_int2c_cmp_internal(a,b) == 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_int2c_less(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_int2c_cmp_internal(a,b) == -1 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}

Datum
complex_int2c_less_equal(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_int2c_cmp_internal(a,b) <= 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_int2c_greater_equal(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_int2c_cmp_internal(a,b) >= 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}


Datum
complex_int2c_greater(PG_FUNCTION_ARGS)
{
	int32       a = PG_GETARG_INT32(0);
        Complex    *b = (Complex *) PG_GETARG_POINTER(1);
        
        if (complex_int2c_cmp_internal(a,b) > 0 ) PG_RETURN_BOOL(true);
        PG_RETURN_BOOL(false);
}

