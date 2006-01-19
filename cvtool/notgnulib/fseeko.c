#include "config.h"

#include <stdio.h>


#ifndef HAVE_FSEEKO
int fseeko(FILE *stream, off_t offset, int whence)
{
    return fseek(stream, offset, whence);
}
#endif
