# Prelude
AC_INIT([arcadia], [1.0])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([autotools])
AM_INIT_AUTOMAKE([foreign -Wall])
# Checks for programs
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR
##LT_INIT
PKG_CHECK_MODULES(QT4, [QtCore QtGui QtSvg QtXml >= 4.4.0], [
  AC_PATH_PROGS(MOC, [moc-qt4 moc], moc,
  		     `eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROG(RCC, rcc, rcc,
  		     `eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROGS(UIC, [uic-qt4 uic], uic,
  		     `eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
], [
  AC_MSG_ERROR([Qt 4 library not found])
])
PKG_CHECK_MODULES(SBML, libsbml >= 5.0.0)
PKG_CHECK_MODULES(GVC, libgvc)
#PKG_CHECK_MODULES(PTHREAD, pthread-stubs)

# Checks for libraries
#AC_CHECK_LIB([sbml]
#AC_LIB_LINKFLAGS([sbml])
#AC_SEARCH_LIBS([gvContext],[gvc])
#AC_LIB_LINKFLAGS([gvc])
#AC_LIB_LINKFLAGS([graph])
#AC_LIB_LINKFLAGS([QtSvg])
#AC_LIB_LINKFLAGS([QtXml])
#AC_LIB_LINKFLAGS([QtGui])
#AC_LIB_LINKFLAGS([QtCore])
#AC_LIB_LINKFLAGS([pthread])
# Output files
AC_CONFIG_FILES([Makefile lib/Makefile src/Makefile])
AC_OUTPUT
