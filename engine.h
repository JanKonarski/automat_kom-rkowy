/*
 *
 * Autor: Jan Konarski
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// typ dla komórki automatu
typedef enum {dead, alive} exist_t;

typedef struct y_node {
	struct y_node* back;
	struct y_node* next;
	
	size_t y;
	exist_t status:1;
} y_list_t;

typedef struct x_node {
	struct x_node* back;
	struct x_node* next;
	
	size_t x;
	size_t y_count;
	y_list_t* rows; // wiersze numerowane od 1
} x_list_t;

typedef struct truss {
	size_t width;
	size_t height;
	
	size_t e_count;
	x_list_t* cols; // kolumny numerowane od 1
} truss_t;

typedef struct node {
	struct node* back;
	struct node* next;
	
	size_t x;
	size_t y;
} position_t;

truss_t* new_truss (size_t width, size_t height);
truss_t* load_truss (char* file_name);
void del_truss (truss_t** tr);
void rand_truss (truss_t** tr, size_t count);
void next_generation (truss_t** tr);
void show_truss (truss_t *tr);

