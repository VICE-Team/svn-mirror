/* not_implemented.h
 *
 * temporary header file for not implemented functions.
 */

#ifndef VICE_NOT_IMPLEMENTED_H
#define VICE_NOT_IMPLEMENTED_H

#include <stdio.h>
#include <stdlib.h>


/** \brief  Not-implemented message with file, function and lineno, calls exit(1)
 */
#define NOT_IMPLEMENTED() \
    fprintf(stderr, \
            "%s:%d: error: function %s() is not implemented yet, exiting\n", \
            __FILE__, __LINE__, __func__); \
    exit(1)

#endif


/** \brief  Not-implemented message with file, function and lineno, only warns
 */
#define NOT_IMPLEMENTED_WARN_ONLY() \
    fprintf(stderr, \
            "%s:%d: warning: function %s() is not implemented yet, continuing\n", \
            __FILE__, __LINE__, __func__)


/** \brief  Incomplete implementation message, only warns
 */
#define INCOMPLETE_IMPLEMENTATION() \
    fprintf(stderr, \
            "%s:%d: warning: function %s() is not fully implemented yet, continuing\n", \
            __FILE__, __LINE__, __func__)

/** \brief  Temporary implementation message, only warns
 */
#define TEMPORARY_IMPLEMENTATION() \
    fprintf(stderr, \
            "%s:%d: warning: function %s() contains a temporary implementation, continuing\n", \
            __FILE__, __LINE__, __func__)
