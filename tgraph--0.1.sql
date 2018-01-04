-- COPYRIGHT (c) Ioannis Tambouras 2011-2015
\echo Use "CREATE EXTENSION tgraph" to load this file. \quit

CREATE FUNCTION similarity_op(text,text)
RETURNS bool
AS '$libdir/tgraph'
LANGUAGE C STRICT STABLE PARALLEL SAFE;  

CREATE OPERATOR % (
        LEFTARG = text,
        RIGHTARG = text,
        PROCEDURE = similarity_op,
        COMMUTATOR = '%',
        RESTRICT = contsel,
        JOIN = contjoinsel
);


