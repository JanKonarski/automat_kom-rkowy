/*
 *
 * Autor: Jan Konarski
 *
 */

#include "engine.h"

static size_t add_y_list (truss_t** tr, size_t x, size_t y)
{
	if ( x > (*tr)->width ||
		 y > (*tr)->height )
		
		return false;
	
	if ( (*tr) == NULL )
		return false;
	
	if ( (*tr)->cols == NULL )
		return false;
		
	while ( (*tr)->cols->next &&
			(*tr)->cols->x != x )
			
		(*tr)->cols = (*tr)->cols->next;
	
		y_list_t* node = (y_list_t*)malloc( sizeof( y_list_t ));
		
		node->back = NULL;
		node->next = NULL;
		node->y = y;
		node->status = alive;
	
		if ( (*tr)->cols->rows == NULL )
			(*tr)->cols->rows = node;
			
		else
		{
			while ( (*tr)->cols->rows->next &&
					(*tr)->cols->rows->y < y ) {
				
				(*tr)->cols->rows = (*tr)->cols->rows->next;
			}			
			
			if ( (*tr)->cols->rows->y == y )
				return false;
			
			if ( (*tr)->cols->rows->y < y &&
				 (*tr)->cols->rows->next == NULL )
			{
				node->back = (*tr)->cols->rows;
				(*tr)->cols->rows->next = node;
				 	
			} else if ( (*tr)->cols->rows->y > y &&
						(*tr)->cols->rows->back == NULL )
			{
				node->next = (*tr)->cols->rows;
				(*tr)->cols->rows->back = node;
					
			} else
			{	
				node->back = (*tr)->cols->rows->back;
				node->next = (*tr)->cols->rows;
				
				((*tr)->cols->rows->back)->next = node;
				(*tr)->cols->rows->back = node;
				
				(*tr)->cols->rows = node;
				
			}
			
			while ( (*tr)->cols->rows->back )
				(*tr)->cols->rows = (*tr)->cols->rows->back;		
		}
		
	++(*tr)->cols->y_count;
	
	while ( (*tr)->cols->back )
		(*tr)->cols = (*tr)->cols->back;
	
	return true;
}

static void add_x_list (truss_t** tr)
{
	size_t i;
	for ( i = 0; i < (*tr)->width; ++i )
	{
		x_list_t* node = (x_list_t*)malloc( sizeof( x_list_t ));
		
		node->back = (*tr)->cols;
		node->next = NULL;
		node->x = i + 1;
		node->y_count = 0;
		node->rows = NULL;
		
		if ( (*tr)->cols )
		{
			while ( (*tr)->cols->next )
				(*tr)->cols = (*tr)->cols->next;
			
			(*tr)->cols->next = node;
		}
		
		(*tr)->cols = node;
	}
	
	while ( (*tr)->cols->back )
		(*tr)->cols = (*tr)->cols->back;
}

truss_t* new_truss (size_t width, size_t height)
{
	if ( width == 0 || height == 0 )
		return NULL;
	
	truss_t* tr = (truss_t*)malloc( sizeof (truss_t ));
	tr->width = width;
	tr->height = height;
	tr->e_count = 0;
	tr->cols = NULL;
	
	add_x_list( &tr );	

	return tr;
}

truss_t* load_truss (char* file_name) {
	
}

void rand_truss (truss_t** tr, size_t count)
{
	size_t width = (*tr)->width;
	size_t height = (*tr)->height;
	if ( count > width * height )
		return;
	
	size_t i = 0;
	while ( i < count )
		if ( !add_y_list( tr,
						  rand() % (*tr)->width + 1,
						  rand() % (*tr)->height + 1 ))
			++i;
}



static void del_y_list (y_list_t** yli, size_t x, size_t y)
{
	
}

static void del_x_list (x_list_t** xli, size_t x)
{
	
}

void del_truss (truss_t** tr)
{
	
}

void next_generation (truss_t** tr)
{
	
}

void show_truss (truss_t *tr)
{
	
}

