\timing off
\c lessons postgres
CREATE EXTENSION IF NOT EXISTS complex;
\c lessons ioannis
begin;
CREATE SCHEMA IF NOT EXISTS  :path;
SET SEARCH_PATH TO :path,public;
SET CLIENT_MIN_MESSAGES = 'ERROR';


CREATE TABLE try (
    age    int ,
    wave   complex
);



end;
\q

