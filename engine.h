/*
 *
 * Autor: Jan Konarski
 *
 */

#pragma once

#include "config.h"
#include "log.h"
#include "strs.h"
#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <png.h>

#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifdef __cplusplus
extern "C" {
#endif



    int count_neigbours(size_t x, size_t y);
    void allocate_mem(size_t width, size_t height);
    void rand_generation();
    void load_generation( const char *file_name );
    void save_generation( const char *file_name );
    void next_generation( Array *a );
    void drop_generation( void );

    void open_image( const char *file_name );
    void save_image( void );
    void close_image( void );

#ifdef __cplusplus
}
#endif

#endif
