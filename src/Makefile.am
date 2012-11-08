
AUTOMAKE_OPTIONS = subdir-objects 

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

AM_CPPFLAGS = ${Cdb_CFLAGS} -DCDB_LOCALEDIR=\"${CDB_LOCALEDIR}\"

bin_PROGRAMS = cdb 

cdb_CPPFLAGS = \
	-I$(includedir)					\
	-DCDB_DATADIR=\""$(pkgdatadir)"\"		\
	$(AM_CPPFLAGS)

cdb_SOURCES = main.cc \
									win_main.cc \
									dialog_preferences.cc \
									config.cc \
									utils.cc

cdb_LDADD = ${Cdb_LIBS}

