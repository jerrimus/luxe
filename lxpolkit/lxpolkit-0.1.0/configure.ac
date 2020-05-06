#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

prog_name = lxpolkit

AC_PREREQ([2.63])
AC_INIT([lxpolkit], [0.1.0], [http://lxde.org/])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
# AM_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# intltool
IT_PROG_INTLTOOL([0.40.0])

# PKG_CHECK_MODULES(XLIB, "x11")
# AC_SUBST(XLIB_CFLAGS)
# AC_SUBST(XLIB_LIBS)

AC_ARG_ENABLE(gtk3,
AC_HELP_STRING([--enable-gtk3],[enable to use gtk+-3.0 instead of gtk+-2.0]),
[case "${enableval}" in
  yes)  enable_gtk3=yes ;;
  no)   enable_gtk3=no ;;
  *) AC_MSG_ERROR([bad value "${enableval}" for --enable-gtk3, use "yes" (default) or "no".]) ;;
esac],[])

# Checks for libraries.
if test "x$enable_gtk3" = "xyes" ; then
  CFLAGS="$CFLAGS -DENABLE_GTK3"
  pkg_modules="gtk+-3.0 >= 3.0.0"
else
  pkg_modules="gtk+-2.0 >= 2.12.0"
fi
PKG_CHECK_MODULES(GTK, [$pkg_modules])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# gio_modules="gthread-2.0 gio-unix-2.0 glib-2.0 >= 2.18.0"
# PKG_CHECK_MODULES(GIO, [$gio_modules])
# AC_SUBST(GIO_CFLAGS)
# AC_SUBST(GIO_LIBS)

# menu_modules="libmenu-cache >= 0.3.2"
# PKG_CHECK_MODULES(MENU_CACHE, [$menu_modules])
# AC_SUBST(MENU_CACHE_CFLAGS)
# AC_SUBST(MENU_CACHE_LIBS)

polkit_modules="polkit-agent-1"
PKG_CHECK_MODULES(POLKIT, [$polkit_modules])
AC_SUBST(POLKIT_CFLAGS)
AC_SUBST(POLKIT_LIBS)


AC_ARG_ENABLE(debug,
	[AC_HELP_STRING([--enable-debug],
		[build libfm with debug support @<:@default=no@:>@])],
	[enable_debug="${enableval}"],
	[enable_debug=no]
)
if test "$enable_debug" = "yes"; then
    # turn on debug and disable optimization
    CPPFLAGS="$CPPFLAGS -DG_ENABLE_DEBUG -O0 -g"
else
    # turn off glib debug checks
    CPPFLAGS="$CPPFLAGS -DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
fi

# Generate po/LINGUAS on the fly rather than relying on translators
# to maintain it manually. This also overcome the problem that Transifex
# cannot add a language to po/LINGUAS if a new po file is submitted.
rm -f $srcdir/po/LINGUAS
for po_file in `ls $srcdir/po/*.po | sort`;
do
    lang=`echo "$po_file" | sed "s|.*/po/\(.*\)\.po|\1|g"`
    echo $lang >> $srcdir/po/LINGUAS
done

GETTEXT_PACKAGE=lxpolkit
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    data/Makefile
    data/ui/Makefile
    po/Makefile.in
])

AC_OUTPUT

if test x"$sysconfdir" != x'/etc'; then
    echo
    echo
    echo 'Error: sysconfdir is not /etc.'
    echo 'Please consider passing --sysconfdir=/etc to configure.';
    echo 'Otherwise default config files will be installed to wrong place.'
    echo
    echo
else
    echo
    echo lxpolkit....................... : Version $VERSION
    echo
    echo Enable GTK3 support.............: "$enable_gtk3"
    echo Enable debug....................: "$enable_debug"
    echo Prefix..........................: $prefix
    echo
    echo The binary will be installed in $prefix/bin
    echo
    echo http://lxde.org/
    echo
    echo "Ready to make lxpolkit. Type 'make' to continue."
    echo
fi
