# Check for Qt compiler flags, linker flags, and binary packages
AC_DEFUN([FIND_MOC],
[
AC_MSG_CHECKING([for moc])
saved_IFS=$IFS
IFS=':'
path_dirs=""
for dir in $PATH; do
    path_dirs="$path_dirs $dir"
done
IFS=$saved_IFS

for moc_dir in $path_dirs $QTDIR/bin; do
    if test -x $moc_dir/moc; then
        MOC=$moc_dir/moc
        break
    fi
done

if test x$MOC = x; then
    AC_MSG_ERROR([*** No Qt meta object compiler (moc) found!])
else
    AC_MSG_RESULT([$MOC])
fi
])

AC_DEFUN([FIND_UIC],
[
AC_MSG_CHECKING([for uic])
saved_IFS=$IFS
IFS=':'
path_dirs=""
for dir in $PATH; do
    path_dirs="$path_dirs $dir"
done
IFS=$saved_IFS

for uic_dir in $path_dirs $QTDIR/bin; do
    if test -x $uic_dir/uic; then
        UIC=$uic_dir/uic
        break
    fi
done

if test x$UIC = x; then
    AC_MSG_ERROR([*** No Qt ui compiler (uic) found!])
else
    AC_MSG_RESULT([$UIC])
fi
])

AC_DEFUN([gw_CHECK_QT],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_PATH_X])

AC_MSG_CHECKING([QTINCDIR])
AC_ARG_WITH([qtincdir], [  --with-qtincdir=DIR        Qt include directory [default=$QTDIR/include]], QTINCDIR=$withval)

# Check that QTINCDIR is defined or that --with-qtincdir given
if test x"$QTINCDIR" = x ; then
    QT_SEARCH="$QTDIR/include /usr/lib64/qt3/include /usr/local/lib64/qt3/include /usr/lib/qt31/include /usr/local/qt31/include /usr/lib/qt3/include /usr/local/qt3/include /usr/lib/qt2/include /usr/local/qt2/include /usr/lib/qt/include /usr/local/qt/include /usr/include/qt /usr/local/include/qt /usr/include/qt3"
    for i in $QT_SEARCH; do
        if test -f $i/qglobal.h -a x$QTINCDIR = x; then QTINCDIR=$i; fi
    done
fi
if test x"$QTINCDIR" = x ; then
    AC_MSG_ERROR([*** QTINCDIR must be defined, or --with-qtincdir option given])
fi
AC_MSG_RESULT([$QTINCDIR])

AC_MSG_CHECKING([QTLIBDIR])
AC_ARG_WITH([qtlibdir], [  --with-qtlibdir=DIR        Qt library directory [default=$QTDIR/lib]], QTLIBDIR=$withval)

# Check that QTLIBDIR is defined or that --with-qtlibdir given
if test x"$QTLIBDIR" = x ; then
    QT_SEARCH="$QTDIR/lib64 $QTDIR/lib  /usr/lib64/qt3/lib64 /usr/local/lib64/qt3/lib64 /usr/lib/qt31/lib /usr/local/qt31/lib /usr/lib/qt3/lib /usr/local/qt3/lib /usr/lib/qt2/lib /usr/local/qt2/lib /usr/lib/qt/lib /usr/local/qt/lib /usr/lib /usr/local/lib"
    for i in $QT_SEARCH; do
        if test -f $i/libqt.so -a x$QTLIBDIR = x; then QTLIBDIR=$i; fi
        if test -f $i/libqt-mt.so -a x$QTLIBDIR = x; then QTLIBDIR=$i; fi
    done
fi
if test x"$QTLIBDIR" = x ; then
    AC_MSG_ERROR([*** QTLIBDIR must be defined, or --with-qtlibdir option given])
fi
AC_MSG_RESULT([$QTLIBDIR])

# Change backslashes in QTDIR to forward slashes to prevent escaping
# problems later on in the build process, mainly for Cygwin build
# environment using MSVC as the compiler
# TODO: Use sed instead of perl
QTDIR=`echo $QTDIR | perl -p -e 's/\\\\/\\//g'`

# Check for moc
FIND_MOC
AC_SUBST(MOC)

# uic is the Qt user interface compiler
FIND_UIC
AC_SUBST(UIC)

# qembed is the Qt data embedding utility.
# It is located in $QTDIR/tools/qembed, and must be compiled and installed
# manually, we'll let it slide if it isn't present
AC_CHECK_PROG(QEMBED, qembed, qembed)

# Calculate Qt include path
QT_CXXFLAGS="-I$QTINCDIR"

QT_IS_EMBEDDED="no"
# On unix, figure out if we're doing a static or dynamic link
case "${host}" in
    *-cygwin)
	AC_DEFINE_UNQUOTED(WIN32, "", Defined if on Win32 platform)
        if test -f "$QTLIBDIR/qt.lib" ; then
            QT_LIB="qt.lib"
            QT_IS_STATIC="yes"
            QT_IS_MT="no"
        elif test -f "$QTLIBDIR/qt-mt.lib" ; then
            QT_LIB="qt-mt.lib" 
            QT_IS_STATIC="yes"
            QT_IS_MT="yes"
        elif test -f "$QTLIBDIR/qt$QT_VER.lib" ; then
            QT_LIB="qt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="no"
        elif test -f "$QTLIBDIR/qt-mt$QT_VER.lib" ; then
            QT_LIB="qt-mt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="yes"
        fi
        ;;

    *)
        QT_IS_STATIC=`ls $QTLIBDIR/*.a 2> /dev/null`
        if test "x$QT_IS_STATIC" = x; then
            QT_IS_STATIC="no"
        else
            QT_IS_STATIC="yes"
        fi
        if test x$QT_IS_STATIC = xno ; then
            QT_IS_DYNAMIC=`ls $QTLIBDIR/*.so 2> /dev/null` 
            if test "x$QT_IS_DYNAMIC" = x;  then
                AC_MSG_ERROR([*** Couldn't find any Qt libraries])
            fi
        fi

        if test "x`ls $QTLIBDIR/libqt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt"
            QT_IS_MT="no"
        elif test "x`ls $QTLIBDIR/libqt-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt-mt"
            QT_IS_MT="yes"
        elif test "x`ls $QTLIBDIR/libqte.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte"
            QT_IS_MT="no"
            QT_IS_EMBEDDED="yes"
        elif test "x`ls $QTLIBDIR/libqte-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte-mt"
            QT_IS_MT="yes"
            QT_IS_EMBEDDED="yes"
        fi
        ;;
esac
AC_MSG_CHECKING([if Qt is static])
AC_MSG_RESULT([$QT_IS_STATIC])
AC_MSG_CHECKING([if Qt is multithreaded])
AC_MSG_RESULT([$QT_IS_MT])
AC_MSG_CHECKING([if Qt is embedded])
AC_MSG_RESULT([$QT_IS_EMBEDDED])

QT_GUILINK=""
QASSISTANTCLIENT_LDADD="-lqassistantclient"
case "${host}" in
    *irix*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *linux*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -ldl -ljpeg"
        fi
        ;;


    *osf*) 
        # Digital Unix (aka DGUX aka Tru64)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *solaris*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -lresolv -lsocket -lnsl"
        fi
        ;;


    *win*)
        # linker flag to suppress console when linking a GUI app on Win32
        QT_GUILINK="/subsystem:windows"

	if test $QT_MAJOR = "3" ; then
	    if test $QT_IS_MT = yes ; then
        	QT_LIBS="/nodefaultlib:libcmt"
            else
            	QT_LIBS="/nodefaultlib:libc"
            fi
        fi

        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS $QT_LIB kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib shell32.lib imm32.lib advapi32.lib wsock32.lib winspool.lib winmm.lib netapi32.lib"
            if test $QT_MAJOR = "3" ; then
                QT_LIBS="$QT_LIBS qtmain.lib"
            fi
        else
            QT_LIBS="$QT_LIBS $QT_LIB"        
            if test $QT_MAJOR = "3" ; then
                QT_CXXFLAGS="$QT_CXXFLAGS -DQT_DLL"
                QT_LIBS="$QT_LIBS qtmain.lib qui.lib user32.lib netapi32.lib"
            fi
        fi
        QASSISTANTCLIENT_LDADD="qassistantclient.lib"
        ;;

esac


if test x"$QT_IS_EMBEDDED" = "xyes" ; then
        QT_CXXFLAGS="-DQWS $QT_CXXFLAGS"
fi

if test x"$QT_IS_MT" = "xyes" ; then
        QT_CXXFLAGS="$QT_CXXFLAGS -D_REENTRANT -DQT_THREAD_SUPPORT"
fi

QT_LDADD="-L$QTLIBDIR $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    OLDLIBS="$LIBS"
    LIBS="$QT_LDADD"
    AC_CHECK_LIB(Xft, XftFontOpen, QT_LDADD="$QT_LDADD -lXft")
    LIBS="$LIBS"
fi

AC_MSG_CHECKING([QT_CXXFLAGS])
AC_MSG_RESULT([$QT_CXXFLAGS])
AC_MSG_CHECKING([QT_LDADD])
AC_MSG_RESULT([$QT_LDADD])

AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_LDADD)
AC_SUBST(QT_GUILINK)
AC_SUBST(QASSISTANTCLIENT_LDADD)

])
dnl End of gw_QT_CHECK

dnl Process this file with autoconf to produce a configure script.
AC_INIT([xdrawchem],[1.9.4],[bherger@users.sourceforge.net])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_SRCDIR(xdrawchem/application.cpp)
AM_CONFIG_HEADER(config.h)

dnl Check for installed OpenBabel, use that if available
PKG_CHECK_MODULES(XDRAWCHEM, openbabel-2.0 >= 2.0.0)
AC_SUBST(XDRAWCHEM_CFLAGS)
AC_SUBST(XDRAWCHEM_LIBS)

#top_builddir=`pwd`
#AC_SUBST(top_builddir)

openbabel_version=1.100.2
AC_SUBST(openbabel_version)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
AC_PATH_PROG(AR, ar)
gw_CHECK_QT

# Set some variable first
CXXFLAGS="-g $CXXFLAGS"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor pow sqrt])

MY_SUBDIRS=". xdrawchem ring doc"
AC_SUBST(MY_SUBDIRS)
AC_CONFIG_FILES([Makefile
		 xdrawchem/Makefile
		 doc/Makefile
		 ring/Makefile])

AC_OUTPUT
