#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.50])
AC_INIT([Optimization of MxM of 2D Matrices on heterogeneous platforms], 1.0, jarico@unex.es)

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])






###############################################################################
# with-taulop
###############################################################################

AC_ARG_WITH(taulop,
[  --with-taulop[[=path]] - Specify the path of the taulop library.],,with_taulop=empty)


if test "$with_taulop" = "empty"; then

  AC_DEFINE_UNQUOTED(TAULOP_FOLDER, $(libdir), [Parameter values are in the default library system folder.])

else

  AC_DEFINE_UNQUOTED(TAULOP_FOLDER, "${with_taulop}", [taulop library is in folder: $with_taulop])

fi

CXXFLAGS=" $CXXFLAGS -I${with_taulop}/include "
LDFLAGS=" $LDFLAGS -L${with_taulop}/lib "







AC_CONFIG_FILES(Makefile src/Makefile)

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT


echo \
"------------------------------------------------------------------------------
------------------------------------------------------------------------------
 ${PACKAGE_NAME} version ${PACKAGE_VERSION}

 General Options

  Prefix . . . . . . . : '${prefix}'
  Compiler . . . . . . : '${CXX} ${CXXFLAGS}'
  With t-Lop lib . . . : '${with_taulop}

 Debug Options


 Now type 'make @<:@<target>@:>@', where the optional <target> is:

  all        - build library
  install    - install everything
  dist       - create the distribution tarball
  tests      - build, but not execute, all tests
------------------------------------------------------------------------------
------------------------------------------------------------------------------"