
AC_PREREQ([2.68])
LT_PREREQ([2.4.2])

#define version as 1.0 for now
AC_INIT([kvmem],[1.0],[saadtalaat@gmail.com])

AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AM_PROG_AR
AC_PROG_LIBTOOL

LT_INIT([-dlopen -static])
AC_SUBST([LIBTOOL_DEPS])

LT_OUTPUT

case "${host_os}" in
    *linux*)
        AC_DEFINE(KVMEM_LINUX,1, [Have Linux OS])
	;;
    *)
	#Well.. That's a port to linux!
        AC_MSG_ERROR([Unsupported system ${host-os}.])
        ;;
esac

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
