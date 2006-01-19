#include "config.h"

#ifndef HAVE_ISBLANK
int isblank(int c)
{
    return (c == ' ' || c == '\t');
}
#endif
