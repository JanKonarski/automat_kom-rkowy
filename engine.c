/*
 *
 * Autor: Jan Konarski
 *
 */

#include "engine.h"

#define alive (int)0xFF
#define death (int)0x00

size_t width_G = 0;
size_t height_G = 0;
const png_byte color_type = PNG_COLOR_TYPE_GRAY;
const png_byte bit_depth = 8;

png_structp png_ptr;
png_infop info_ptr;
png_bytep *truss_ptrs;

FILE *img_ptr;
char *img_name;



void initArray(Array *a, size_t initialSize) {
    a->array = (int *)malloc(initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array *a, int x, int y) {
    if (a->used == a->size) {
        a->size *= 2;
        a->array = (int *)realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = x;
    a->array[a->used++] = y;
}

void freeArray(Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void allocate_mem(size_t width, size_t height)
{
    width_G = width;
    height_G = height;

    truss_ptrs = (png_bytep*)malloc( sizeof( png_bytep ) * width_G );

    size_t x, y;
    for (x = 0; x < width_G; ++x)
    {
        truss_ptrs[x] = (png_byte*)malloc( sizeof( png_byte ) * height_G );

        for(y = 0; y < width_G; ++y)
            truss_ptrs[x][y]=alive;
    }

}

void rand_generation()
{
    srand( time( NULL ));

    size_t x, y;
    for (x = 0; x < width_G; ++x)
        for (y = 0; y < height_G; ++y)
            truss_ptrs[x][y] = rand() % 14 ? alive : death;
}

void load_generation( const char *file_name )
{
    FILE *fp;
    fp = fopen(file_name, "r");

    int x, y;
    if (fscanf(fp,"%d %d",&x, &y)==2)
        allocate_mem(x,y);

    while (fscanf(fp,"%d %d",&x, &y)==2)
        truss_ptrs[x][y]=death;

}

void save_generation( const char *file_name )
{
    FILE * fp;

    fp = fopen (file_name, "w+");
    fprintf(fp, "%d %d \n", width_G, height_G);
    size_t x, y;
    for (x = 0; x < width_G; ++x)
        for(y = 0; y < width_G; ++y)
            if(truss_ptrs[x][y]==death)
                fprintf(fp, "%d %d \n", x,y);
    fclose(fp);
}

int count_neigbours(size_t x, size_t y)
{
    int neighbours = 0;
    if( x > 0)
    {
        if( truss_ptrs[x-1][y]==death)      neighbours++;

        if(y>0)
            if( truss_ptrs[x-1][y-1]==death)    neighbours++;
        if(y+1<height_G)
            if( truss_ptrs[x-1][y+1]==death)    neighbours++;
    }

    if( x+1 < width_G)
    {
        if( truss_ptrs[x+1][y]==death)      neighbours++;

        if(y > 0)
            if( truss_ptrs[x+1][y-1]==death)    neighbours++;
        if( y+1 < height_G)
            if( truss_ptrs[x+1][y+1]==death)    neighbours++;
    }

    if( y > 0)
        if( truss_ptrs[x][y-1]==death)      neighbours++;
    if( y+1 < height_G)
        if( truss_ptrs[x][y+1]==death)      neighbours++;

    return neighbours;
}


void next_generation( Array *a )
{
    size_t x, y;
    for (x = 0; x < width_G; ++x)
    {
        for (y = 0; y < height_G; ++y)
        {
            int neighbours = count_neigbours(x,y);

            if(truss_ptrs[x][y]==alive && neighbours==3)
                insertArray(a, x, y);

            if(truss_ptrs[x][y]==death && (neighbours!=3 && neighbours!=2))
                insertArray(a, x, y);
        }
    }

    for(size_t i=0; i < a->used; i+=2)
    {
        if(truss_ptrs[a->array[i]][a->array[i+1]]==alive)
            truss_ptrs[a->array[i]][a->array[i+1]]=death;
        else
            truss_ptrs[a->array[i]][a->array[i+1]]=alive;
    }
    a->used=0;

}

void drop_generation( void )
{
    size_t x;
    for (x = 0; x < width_G; x++)
        free( truss_ptrs[x] );
    free( truss_ptrs );
}

void open_image( const char *file_name )
{
    img_ptr = fopen( file_name, "wb" );
    if ( !img_ptr ) // log fail
        logs( ERROR,
            strjoin( "File ", strjoin( file_name, " couldn't be opened" )));
    img_name = file_name;
}

void save_image( void )
{
    png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if ( !png_ptr )  // log fail
        logs( ERROR, "png_create_write_struct failed" );

    info_ptr = png_create_info_struct( png_ptr );
    if ( !info_ptr ) // log fail
        logs( ERROR, "png_create_info_struct failed" );

    if ( setjmp( png_jmpbuf( png_ptr ))) // log fail
        logs( ERROR, "Error during init_io" );

    png_init_io( png_ptr, img_ptr );

    if ( setjmp( png_jmpbuf( png_ptr ))) // log fail
        logs( ERROR, "Error during writing header" );

    png_set_IHDR( png_ptr, info_ptr, height_G, width_G,
                  bit_depth, color_type, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

    png_write_info( png_ptr, info_ptr );

    if ( setjmp( png_jmpbuf( png_ptr ))) // log fail
        logs( ERROR, "Error during writing bytes" );

    png_write_image( png_ptr, truss_ptrs );

    if ( setjmp( png_jmpbuf( png_ptr ))) // log fail
        logs( ERROR, "Error during end of write" );

    png_write_end( png_ptr, NULL );
}

void close_image( void )
{
    fclose( img_ptr );
    if ( !img_ptr ) // log info
        logs( ERROR,
            strjoin( "File ", strjoin( img_name, " couldn't be closed" )));
}
