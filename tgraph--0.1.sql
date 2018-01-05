-- COPYRIGHT (c) Ioannis Tambouras 2011-2015
\echo Use "CREATE EXTENSION tgraph" to load this file. \quit

CREATE FUNCTION similarity_op(text,text)
RETURNS bool
AS '$libdir/tgraph'
LANGUAGE C STRICT STABLE PARALLEL SAFE;  

CREATE FUNCTION gin_extract_value_trgm(text, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION gin_extract_query_trgm(text, internal, int2, internal, internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION gin_trgm_consistent(internal, int2, text, int4, internal, internal, internal, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;


CREATE OPERATOR % (
        LEFTARG = text,
        RIGHTARG = text,
        PROCEDURE = similarity_op,
        COMMUTATOR = '%',
        RESTRICT = contsel,
        JOIN = contjoinsel
);


CREATE OPERATOR CLASS gin_trgm_ops
FOR TYPE text USING gin
AS
        OPERATOR        1       % (text, text),
        FUNCTION        1       btint4cmp (int4, int4),
        FUNCTION        2       gin_extract_value_trgm (text, internal),
        FUNCTION        3       gin_extract_query_trgm (text, internal, int2, internal, internal, internal, internal),
        FUNCTION        4       gin_trgm_consistent (internal, int2, text, int4, internal, internal, internal, internal),
        STORAGE         int4;


