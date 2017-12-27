-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION complex" to load this file. \quit


DROP OPERATOR IF EXISTS && ;

DROP OPERATOR CLASS IF EXISTS  complex_inclusion_ops USING brin;

