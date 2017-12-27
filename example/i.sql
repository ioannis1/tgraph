\timing off
SET SEARCH_PATH TO  :path,public;
SET CLIENT_MIN_MESSAGES = 'ERROR';
begin;


CREATE OPERATOR CLASS cincl_ops
  DEFAULT FOR TYPE complex
  USING brin   AS
    OPERATOR      3        @>(complex,complex),
    FUNCTION      1        brin_inclusion_opcinfo(internal) ,
    FUNCTION      2        brin_inclusion_add_value(internal, internal, internal, internal) ,
    FUNCTION      3        brin_inclusion_consistent(internal, internal, internal) ,
    FUNCTION      4        brin_inclusion_union(internal, internal, internal) 

\q

--set enable_seqscan = off;
--set enable_indexscan = off;

EXPLAIN (analyze)
SELECT wave 
FROM try
WHERE  wave = '(33,33)'
--     AND age > wave 
;

\q

