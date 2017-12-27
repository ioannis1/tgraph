-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION complex" to load this file. \quit



CREATE OPERATOR && (
    LEFTARG   = complex,
    RIGHTARG  = complex,
    PROCEDURE = complex_overlaps
);

CREATE OPERATOR CLASS complex_inclusion_ops
  FOR TYPE complex
  USING brin  AS
    OPERATOR      3        &&(complex, complex),
    FUNCTION      1        brin_inclusion_opcinfo(internal) ,
    FUNCTION      2        brin_inclusion_add_value(internal, internal, internal, internal) ,
    FUNCTION      3        brin_inclusion_consistent(internal, internal, internal) ,
    FUNCTION      4        brin_inclusion_union(internal, internal, internal) 
;
