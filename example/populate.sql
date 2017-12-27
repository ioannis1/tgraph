\timing off
SET SEARCH_PATH TO :path,public;

INSERT INTO try
SELECT --public.random_string(4)
       i
     ,( '('|| i ||','|| i || ')')::complex
FROM  generate_series(1,100000) a(i);

ANALYZE try;

\q


\COPY try FROM stdin;
1	(1,3)
2	(2,4)
\.

\q

