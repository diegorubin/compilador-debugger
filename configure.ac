AC_INIT([Gnomato],[0.1],[rubin.diego@gmail.com])

AM_INIT_AUTOMAKE(cdb,0.1)

AC_PROG_CC
AC_PROG_CXX

AC_PROG_INSTALL

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES([Cdb],[gtkmm-3.0 >= 3.0.0, libnotify >= 0.7.3 ])

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=cdb
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext domain])

AM_GLIB_GNU_GETTEXT

CDB_LOCALEDIR=[${datadir}/locale]
AC_SUBST(CDB_LOCALEDIR)

AC_CONFIG_FILES([
Makefile
src/Makefile
data/Makefile
po/Makefile.in
])

AC_OUTPUT(cdb.desktop)

