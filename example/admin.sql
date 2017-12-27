\timing off
SET SEARCH_PATH TO :path;
SET CLIENT_MIN_MESSAGES = 'ERROR';


CREATE INDEX wave_idx ON try USING brin (wave complex_minimax_ops);


