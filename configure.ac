# Process this file with autoconf to produce a configure script -----

AC_PREREQ(2.57)
AC_INIT(djvu, 1.0)
AC_CONFIG_SRCDIR(src/pdf_renderer.cpp)

AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS(config.h)

# TODO get to know what it's for    
#AM_GCONF_SOURCE_2

dnl ------------------------
dnl Setup Plugin directories
dnl ------------------------
uds_plugin_dir='${libdir}/uds'
uds_data_dir='${datadir}/uds'
AC_SUBST(uds_plugin_dir)
AC_SUBST(uds_data_dir)

# Checks for programs ---------------------------------------------
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_STDC_HEADERS
AC_PROG_INSTALL

# Args -------------------------------------------------------------

AC_ARG_ENABLE(debug,
  [  --enable-debug          enable debug ( verbose ) build],
     enable_debug=$enableval, enable_debug=no )

PKG_CHECK_MODULES(DEPS, gtk+-2.0 >= 2.2 glib-2.0 >= 2.2 gobject-2.0 >= 2.0 gmodule-2.0 gthread-2.0 gtk+-x11-2.0 >= 2.6 dbus-glib-1 >= 0.70 pangoft2 >= 1.17.4 liberipc >= 1.7)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

PKG_CHECK_MODULES(ddjvuapi, ddjvuapi >= 3.5.22,,AC_MSG_ERROR([*** Required djvulibre (ddjvuapi) not installed ***]))
AC_SUBST(ddjvuapi_CFLAGS)
AC_SUBST(ddjvuapi_LIBS)

AC_OUTPUT([
Makefile
plugin_impl/Makefile
])

TEMP_LTFILE=`echo $LIBTOOL | tr '/' ' ' | awk '{ print $3 }'`
AC_MSG_RESULT(patching $TEMP_LTFILE to fix HIDEOUS RPATH ISSUE)
sed -i -r 's/(hardcode_into_libs)=.*$/\1=no/' $TEMP_LTFILE

AC_MSG_RESULT(patching Makefiles to fix EXPORT-DYNAMIC ISSUE)
sed -i "s/-Wl,--export-dynamic//" Makefile
sed -i "s/-Wl,--export-dynamic//" plugin_impl/Makefile

# ==========================================================================
echo "
                 Universal Document Shell Plugins $VERSION
                 =========================================

                 plugin name:                    $PACKAGE_NAME
                 prefix:                         ${prefix}
                 source code location:           ${srcdir}
                 Building with Debug:            ${enable_debug}

"
