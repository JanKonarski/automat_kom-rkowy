/*
 *
 * Autor: Jan Konarski
 *
 */

#include "engine.h"

static void add_y_list (truss_t** tr, size_t x, size_t y) {
	
}

static void add_x_list (truss_t** tr) {
	size_t i;
	for (i = 0; i < (*tr)->width; ++i) {
		x_list_t* node = (x_list_t*)malloc( sizeof(x_list_t) );
		
		node->back = ((*tr)->cols);
		node->next = NULL;
		node->x = i + 1;
		node->y_count = 0;
		node->rows = NULL;
		
		if ( (*tr)->cols ) {
			while ( ((*tr)->cols)->next )
				((*tr)->cols) = ((*tr)->cols)->next;
			
			((*tr)->cols)->next = node;
		}
		
		((*tr)->cols) = node;
	}
	
	while ( ((*tr)->cols)->back )
		((*tr)->cols) = ((*tr)->cols)->back;
}

truss_t* new_truss (size_t width, size_t height) {
	if ( width == 0 || height == 0 )
		return NULL;
	
	truss_t* tr = (truss_t*)malloc( sizeof(truss_t) );
	tr->width = width;
	tr->height = height;
	tr->e_count = 0;
	tr->cols = NULL;
	
	add_x_list(&tr);
	
	return tr;
}

truss_t* load_truss (char* file_name) {
	
}

void rand_truss (truss_t** tr, size_t count) {
	
}

static void del_y_list (y_list_t** yli, size_t x, size_t y) {
	
}

static void del_x_list (x_list_t** xli, size_t x) {
	
}

void del_truss (truss_t** tr) {
	
}

