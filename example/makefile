.PHONY: $(shell ls)
.SILENT:
.DEFAULT_GOAL = mini
.DEFAULT_GOAL = reload
.DEFAULT_GOAL = i

host = localhost
port = $(if $(PGPORT),$(PGPORT),5434)
user = ioannis
db   = lessons

conn = -d "host=$(host) port=$(port) dbname=$(db) user=$(user) "
conn = -h       $(host) -d   $(port)  -d    $(db)  -U  $(user) 


opt = '--search_path=wave'


des dew det d dt dx di:
	PGOPTIONS=$(opt) psql -q $(conn) -c '\di+ wave.*'

mini: drop create
drop: 
	psql -q $(conn) -U postgres -v path=wave  <  $@.sql
create populate admin select i:
	psql -q $(conn) -v path=wave  <  $@.sql
reload: drop create populate admin select
pg:
	PGOPTIONS=$(opt) psql -q $(conn)
ver:
	PGOPTIONS=$(opt) psql -tq $(conn) -c 'select current_database(), inet_server_port()'

vi:
	vi i.sql

