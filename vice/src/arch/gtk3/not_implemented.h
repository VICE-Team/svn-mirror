/* not_implemented.h
 *
 * temporary header file for not implemented functions.
 */

#ifndef VICE_NOT_IMPLEMENTED_H
#define VICE_NOT_IMPLEMENTED_H

#include <stdlib.h>


/** \brief  Not-implemented message with file, function and lineno, calls exit(1)
 */
#define NOT_IMPLEMENTED() \
    fprintf(stderr, \
            "ERROR: function %s in file() %s at line %d not implemented yet, exiting\n", \
            __func__, __FILE__, __LINE__); \
    exit(1)

#endif


/** \brief  Not-implemented message with file, function and lineno, only warns
 */
#define NOT_IMPLEMENTED_WARN_ONLY() \
    fprintf(stderr, \
            "WARNING: function %s() in file %s at line %d not implemented yet, continuing\n", \
            __func__, __FILE__, __LINE__);

