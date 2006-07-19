#ifndef NOTGNULIB_ISBLANK_H
#define NOTGNULIB_ISBLANK_H

#include "config.h"

#include <ctype.h>

#ifndef HAVE_ISBLANK
int isblank(int c);
#endif

#endif
