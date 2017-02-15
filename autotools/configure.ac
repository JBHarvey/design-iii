AC_INIT([Typhoon], [1.0], [no-reply@design-iii.ulaval.ca])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CHECK_LIB([criterion], [criterion_initialize], [], [
              AC_MSG_ERROR([unable to find Criterion :C])
              ], [])

AC_CHECK_LIB([opencv_core], [opencv_imgproc], [], [
              AC_MSG_ERROR([unable to find opencv :C])
              ], [])

AC_PROG_AWK
AC_PROG_CC

AC_CONFIG_FILES([Makefile])

AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_OUTPUT
