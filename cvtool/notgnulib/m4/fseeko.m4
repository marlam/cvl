AC_DEFUN([ngl_FUNC_FSEEKO],
[
  AC_CHECK_TYPE([off_t],, 
    [AC_DEFINE([off_t], [long], [Define to `long' if `off_t' is missing.])], 
    [#include <sys/types.h>
     #include <stdio.h>])
  AC_CHECK_FUNCS(fseeko)
])
