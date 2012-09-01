PHP_ARG_ENABLE(opencl, whether to enable opencl functions,
[  --enable-opencl         Enable opencl support])

if test "$PHP_OPENCL" != "no"; then
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES"

  AC_MSG_CHECKING(PHP version)
  AC_TRY_COMPILE([#include <php_version.h>], [
#if !defined(PHP_VERSION_ID) || PHP_VERSION_ID < 50300
#error  this extension requires at least PHP version 5.3.0
#endif
],
[AC_MSG_RESULT(ok)],
[AC_MSG_ERROR([need at least PHP 5.3.0])])

  export CPPFLAGS="$OLD_CPPFLAGS"

  export OLD_LIBS="$LIBS"
  export LIBS="$OLD_LIBS"

  if test `uname -s` = "Darwin"; then
    PHP_CHECK_FRAMEWORK(OpenCL, clGetPlatformIDs,
      [],[
        AC_MSG_ERROR([OpenCL framework is not available])
      ])
  else
    PHP_CHECK_LIBRARY(OpenCL, clGetPlatformIDs,
      [
        PHP_EVAL_LIBLINE([-lOpenCL], OPENCL_SHARED_LIBADD)
      ],[
        AC_MSG_ERROR([OpenCL library is not available])
      ],[
        -lOpenCL
      ])
  fi

  export LIBS="$OLD_LIBS"

  PHPCL_SOURCES="opencl.c platform.c device.c context.c queue.c memory.c"
  PHPCL_SOURCES="$PHPCL_SOURCES program.c kernel.c event.c sampler.c"
  PHP_SUBST(OPENCL_SHARED_LIBADD)
  PHP_NEW_EXTENSION(opencl, $PHPCL_SOURCES, $ext_shared)
fi
