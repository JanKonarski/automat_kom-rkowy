#include <stdlib.h>


#ifndef _ARRAY_H_
#define _ARRAY_H_

typedef struct {
    int *array;
    size_t used;
    size_t size;
    } Array;


    void initArray(Array *a, size_t initialSize);
    void insertArray(Array *a, int x, int y);
    void freeArray(Array *a);


#endif
