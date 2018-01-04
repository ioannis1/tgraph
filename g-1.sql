/* contrib/btree_gin/btree_gin--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION btree_gin" to load this file. \quit

CREATE FUNCTION gin_btree_consistent(internal, int2, anyelement, int4, internal, internal) RETURNS bool
CREATE FUNCTION gin_extract_value_int2(int2, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_int2(int2, int2, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_int2(int2, internal, int2, internal, internal) RETURNS internal

CREATE OPERATOR CLASS int2_ops
DEFAULT FOR TYPE int2 USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    //FUNCTION        1       btint2cmp(int2,int2),
    FUNCTION        2       gin_extract_value_int2(int2, internal),
    FUNCTION        3       gin_extract_query_int2(int2, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_int2(int2,int2,int2, internal),
STORAGE         int2;

CREATE FUNCTION gin_extract_value_int4(int4, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_int4(int4, int4, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_int4(int4, internal, int2, internal, internal) RETURNS internal

CREATE OPERATOR CLASS int4_ops
DEFAULT FOR TYPE int4 USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       btint4cmp(int4,int4),
    FUNCTION        2       gin_extract_value_int4(int4, internal),
    FUNCTION        3       gin_extract_query_int4(int4, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_int4(int4,int4,int2, internal),
STORAGE         int4;

CREATE FUNCTION gin_extract_value_int8(int8, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE FUNCTION gin_compare_prefix_int8(int8, int8, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_int8(int8, internal, int2, internal, internal) RETURNS internal

CREATE FUNCTION gin_extract_value_float4(float4, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_float4(float4, float4, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_float4(float4, internal, int2, internal, internal) RETURNS internal

CREATE FUNCTION gin_extract_value_float8(float8, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_float8(float8, float8, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_float8(float8, internal, int2, internal, internal) RETURNS internal
CREATE FUNCTION gin_extract_value_money(money, internal) RETURNS internal AS 'MODULE_PATHNAME'
CREATE FUNCTION gin_compare_prefix_money(money, money, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_money(money, internal, int2, internal, internal) RETURNS internal

CREATE FUNCTION gin_extract_value_oid(oid, internal) RETURNS internal AS 'MODULE_PATHNAME'
CREATE FUNCTION gin_compare_prefix_oid(oid, oid, int2, internal) RETURNS int4

CREATE FUNCTION gin_extract_query_oid(oid, internal, int2, internal, internal) RETURNS internal
CREATE OPERATOR CLASS oid_ops
DEFAULT FOR TYPE oid USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       btoidcmp(oid,oid),
    FUNCTION        2       gin_extract_value_oid(oid, internal),
    FUNCTION        3       gin_extract_query_oid(oid, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_oid(oid,oid,int2, internal),
STORAGE         oid;

CREATE FUNCTION gin_extract_value_timestamp(timestamp, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_timestamp(timestamp, timestamp, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_timestamp(timestamp, internal, int2, internal, internal) RETURNS internal
CREATE OPERATOR CLASS timestamp_ops
DEFAULT FOR TYPE timestamp USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       timestamp_cmp(timestamp,timestamp),
    FUNCTION        2       gin_extract_value_timestamp(timestamp, internal),
    FUNCTION        3       gin_extract_query_timestamp(timestamp, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_timestamp(timestamp,timestamp,int2, internal),
STORAGE         timestamp;

CREATE FUNCTION gin_extract_value_timestamptz(timestamptz, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_timestamptz(timestamptz, timestamptz, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_timestamptz(timestamptz, internal, int2, internal, internal) RETURNS internal
CREATE OPERATOR CLASS timestamptz_ops
DEFAULT FOR TYPE timestamptz USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       timestamptz_cmp(timestamptz,timestamptz),
    FUNCTION        2       gin_extract_value_timestamptz(timestamptz, internal),
    FUNCTION        3       gin_extract_query_timestamptz(timestamptz, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_timestamptz(timestamptz,timestamptz,int2, internal),
STORAGE         timestamptz;

CREATE FUNCTION gin_extract_value_time(time, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_time(time, time, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_time(time, internal, int2, internal, internal) RETURNS internal
CREATE OPERATOR CLASS time_ops
DEFAULT FOR TYPE time USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       time_cmp(time,time),
    FUNCTION        2       gin_extract_value_time(time, internal),
    FUNCTION        3       gin_extract_query_time(time, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_time(time,time,int2, internal),
STORAGE         time;


CREATE FUNCTION gin_extract_value_numeric(numeric, internal) RETURNS internal
CREATE FUNCTION gin_compare_prefix_numeric(numeric, numeric, int2, internal) RETURNS int4
CREATE FUNCTION gin_extract_query_numeric(numeric, internal, int2, internal, internal) RETURNS internal
CREATE FUNCTION gin_numeric_cmp(numeric, numeric) RETURNS int4

CREATE OPERATOR CLASS numeric_ops
DEFAULT FOR TYPE numeric USING gin
AS
    OPERATOR        1       <,
    OPERATOR        2       <=,
    OPERATOR        3       =,
    OPERATOR        4       >=,
    OPERATOR        5       >,
    FUNCTION        1       gin_numeric_cmp(numeric,numeric),
    FUNCTION        2       gin_extract_value_numeric(numeric, internal),
    FUNCTION        3       gin_extract_query_numeric(numeric, internal, int2, internal, internal),
    FUNCTION        4       gin_btree_consistent(internal, int2, anyelement, int4, internal, internal),
    FUNCTION        5       gin_compare_prefix_numeric(numeric,numeric,int2, internal),
STORAGE         numeric;
