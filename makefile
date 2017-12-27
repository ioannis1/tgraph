EXTENSION = complex             # the extensions name
DATA      = complex--0.0.1.sql  complex--0.0.1--0.0.2.sql   complex--0.0.2--0.0.1.sql
REGRESS   = complex_test        # our test script file (without extension)
MODULES   = complex             # our c module file to build

# postgres build stuff
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

vi:
	 vi ~postgres/src/postgres/src/tutorial/complex.c
ins:
	sudo -u postgres make install
i drop:
	psql -qX -h localhost -d contrib_regression -U postgres  < $@.sql
c:
	PGUSER=postgres sudo -E  make  installcheck
pgxs:
	vi $(PGXS)
