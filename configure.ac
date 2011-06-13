AC_INIT([gtk-wordnet-browser], [0.1], [eee@someuser.com])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])


GLIB_REQUIRED=2.28.0
GTK_REQUIRED=2.24.4

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= $GLIB_REQUIRED])
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= $GTK_REQUIRED])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_CHECK_HEADER([wn.h], [],
                [AC_MSG_ERROR([WordNet header "wn.h" was not found])])
AC_CHECK_LIB([wordnet], [wninit], [],
             AC_MSG_ERROR([WordNet library was not found]))

AC_CONFIG_FILES([
        Makefile
        src/Makefile
])
AC_OUTPUT
