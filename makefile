# -- COPYRIGHT (c) Ioannis Tambouras 2011-2015

EXTENSION  = tgraph        
DATA       = tgraph--0.1.sql
REGRESS    = tgraph_test  
MODULES    = tgraph      
OBJS       = tgraph.o tgraph_ops.o  trgm_gin.o trgm_regexp.o 
PGFILEDESC = "tgraph - trigram matching"

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

vi:
	 vi ~postgres/src/postgres/src/tutorial/tgraph.c
ins:
	sudo -u postgres make install
i drop:
	psql -qX -h localhost -d lessons -U postgres  < $@.sql
c:
	PGUSER=postgres sudo -E  make  installcheck
pgxs:
	vi $(PGXS)


