#ifndef NOTGNULIB_FSEEKO_H
#define NOTGNULIB_FSEEKO_H

#include "config.h"

#include <stdio.h>


#ifndef HAVE_FSEEKO
int fseeko(FILE *stream, off_t offset, int whence);
#endif

#endif
