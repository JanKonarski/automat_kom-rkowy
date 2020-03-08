/*
 *
 * Autor: Jan Konarski
 *
 */

#pragma once

#include "config.h"

#include <stdio.h>
#include <time.h>

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
