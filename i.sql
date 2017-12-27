SET CLIENT_MIN_MESSAGES = 'ERROR';
SET SEARCH_PATH to public;
BEGIN;

SELECT complex_mult( '(2,7)'::complex, '(3,9)');

\q
CREATE OPERATOR && (
    LEFTARG   = complex,
    RIGHTARG  = complex,
    PROCEDURE = complex_overlaps
);

CREATE OPERATOR CLASS complex_inclusion_ops
  DEFAULT FOR TYPE complex
  USING brin  AS
    OPERATOR      3        &&(complex, complex),
    FUNCTION      1        brin_inclusion_opcinfo(internal) ,
    FUNCTION      2        brin_inclusion_add_value(internal, internal, internal, internal) ,
    FUNCTION      3        brin_inclusion_consistent(internal, internal, internal) ,
    FUNCTION      4        brin_inclusion_union(internal, internal, internal) 
;
