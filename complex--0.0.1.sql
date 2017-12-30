-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION complex" to load this file. \quit

CREATE TYPE complex;

CREATE OR REPLACE FUNCTION complex_in(cstring)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION complex_out(complex)
    RETURNS cstring
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION complex_recv(internal)
   RETURNS complex
    AS '$libdir/complex'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_send(complex)
   RETURNS bytea
    AS '$libdir/complex'
   LANGUAGE C IMMUTABLE STRICT;


CREATE TYPE complex (
   internallength = 8,
   input          = complex_in,
   output         = complex_out,
   receive        = complex_recv,
   send           = complex_send,
   alignment      = float
);

CREATE OR REPLACE FUNCTION complex_abs_cmp(complex,complex)
   RETURNS int
    AS '$libdir/complex'
   LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION complex_add(complex, complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_subtract(complex, complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_dot(complex, complex)
    RETURNS float4
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mag(complex)
    RETURNS real
    AS '$libdir/complex', 'complex_mag'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_mag_squared(complex)
    RETURNS real
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_equal(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_less(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_less_equal(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_greater_equal(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_greater(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_conjugate(complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_mult(complex,complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_polar(complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_xy(complex)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR | (
    rightarg = complex,
    procedure = mag
);
CREATE OPERATOR |^ (
    rightarg = complex,
    procedure = complex_mag_squared
);
CREATE OPERATOR ^ (
    rightarg = complex,
    procedure = complex_polar
);
CREATE OPERATOR @ (
    leftarg = complex,
    procedure = complex_conjugate
);
CREATE OPERATOR + (
    leftarg = complex,
    rightarg = complex,
    procedure = complex_add,
    commutator = +
);

CREATE OPERATOR - (
    leftarg = complex,
    rightarg = complex,
    procedure = complex_subtract
);
CREATE OPERATOR * (
    leftarg = complex,
    rightarg = complex,
    procedure = complex_mult
);

CREATE OPERATOR < (
    leftarg    = complex,
    rightarg   = complex,
    procedure  = complex_less,
    commutator = >  ,
    restrict   = scalarltsel,
    join       = scalarltjoinsel
);

CREATE OPERATOR <= (
    leftarg    = complex,
    rightarg   = complex,
    procedure  = complex_less_equal,
    commutator = >=  ,
    restrict   = scalarltsel,
    join       = scalarltjoinsel
);

CREATE OPERATOR = (
    leftarg    = complex,
    rightarg   = complex,
    procedure  = complex_equal,
    commutator = = ,
    restrict   = eqsel,
    join       = eqjoinsel
);


CREATE OPERATOR >= (
    leftarg    = complex,
    rightarg   = complex,
    procedure  = complex_greater_equal,
    commutator = <= ,
    restrict   = scalargtsel,
    join       = scalargtjoinsel
);

CREATE OPERATOR > (
    leftarg    = complex,
    rightarg   = complex,
    procedure  = complex_greater,
    commutator = < ,
    restrict   = scalargtsel,
    join       = scalargtjoinsel
);

CREATE AGGREGATE complex_sum (
   sfunc = complex_add,
   basetype = complex,
   stype = complex,
   initcond = '(0,0)'
);


CREATE OPERATOR CLASS complex_abs_ops DEFAULT FOR TYPE complex USING btree AS 
        OPERATOR 1 <  ,
        OPERATOR 2 <= ,
        OPERATOR 3 =  ,
        OPERATOR 4 >= ,
        OPERATOR 5 >  ,
        FUNCTION 1  complex_abs_cmp(complex,complex)
;

CREATE OR REPLACE FUNCTION complex_int2c_cmp(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_int2c_equal(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_int2c_less(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_int2c_less_equal(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_int2c_greater_equal(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_int2c_greater(int,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_overlaps(complex,complex)
    RETURNS boolean
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_real(complex)
    RETURNS float4
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_img(complex)
    RETURNS float4
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_new(float4,float4)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_theta(complex)
    RETURNS float4
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION complex_theta_add(complex,float4)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION complex_new_polar(float4,float4)
    RETURNS complex
    AS '$libdir/complex'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OPERATOR < (
    leftarg    = int,
    rightarg   = complex,
    procedure  = complex_int2c_less,
    commutator = >  ,
    restrict   = scalarltsel,
    join       = scalarltjoinsel
);

CREATE OPERATOR <= (
    leftarg    = int,
    rightarg   = complex,
    procedure  = complex_int2c_less_equal,
    commutator = >=  ,
    restrict   = scalarltsel,
    join       = scalarltjoinsel
);

CREATE OPERATOR = (
    leftarg    = int,
    rightarg   = complex,
    procedure  = complex_int2c_equal,
    commutator = = ,
    restrict   = eqsel,
    join       = eqjoinsel
);


CREATE OPERATOR >= (
    leftarg    = int,
    rightarg   = complex,
    procedure  = complex_int2c_greater_equal,
    commutator = <= ,
    restrict   = scalargtsel,
    join       = scalargtjoinsel
);

CREATE OPERATOR > (
    leftarg    = int,
    rightarg   = complex,
    procedure  = complex_int2c_greater,
    commutator = < ,
    restrict   = scalargtsel,
    join       = scalargtjoinsel
);

CREATE OPERATOR CLASS complex_minimax_ops DEFAULT FOR TYPE complex USING brin AS
        FUNCTION 1      brin_minmax_opcinfo(internal) ,
        FUNCTION 2      brin_minmax_add_value(internal,internal,internal,internal) ,
        FUNCTION 3      brin_minmax_consistent(internal,internal,internal) ,
        FUNCTION 4      brin_minmax_union(internal,internal,internal) ,
        OPERATOR 1 <  ,
        OPERATOR 2 <= ,
        OPERATOR 3 =  ,
        OPERATOR 4 >= ,
        OPERATOR 5 >
;

