/*
 *
 * Autor: Jan Konarski
 *
 */
 /*
  *
  * Autor: Jan Konarski
  * Autor: Maciek Ko³ek
  *
  */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum { INFO, WARN, ERROR, DEBUG } logtype_t;

    void logs( logtype_t lt, const char *msg );

#ifdef __cplsuplus
}
#endif

#endif
