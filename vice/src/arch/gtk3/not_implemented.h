/* not_implemented.h
 *
 * temporary header file for not implemented functions.
 */

#ifndef VICE_NOT_IMPLEMENTED_H
#define VICE_NOT_IMPLEMENTED_H

#include <stdlib.h>

#define NOT_IMPLEMENTED() \
    fprintf(stderr, "Function %s in file %s at line %d not implemented yet\n", \
            __func__, __FILE__, __LINE__); \
    exit(1)

#endif


