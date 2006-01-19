AC_DEFUN([ngl_FUNC_C99MATH],
[
  AC_CHECK_LIB(m, cos)
  AC_CHECK_FUNCS(lround fmin fmax)
])
