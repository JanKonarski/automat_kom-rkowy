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

void rand_generation( size_t width, size_t height )
{
    width_G = width;
    height_G = height;

    truss_ptrs = (png_bytep*)malloc( sizeof( png_bytep ) * width_G );

    srand( time( NULL ));

    size_t x, y;
    for (x = 0; x < width_G; ++x)
    {
        truss_ptrs[x] = (png_byte*)malloc( sizeof( png_byte ) * height_G );

        for (y = 0; y < height_G; ++y)
        {
            truss_ptrs[x][y] = rand() % 2 ? alive : death;
        }
    }
}

void load_generation( const char *file_name )
{

}

void save_generation( const char *file_name )
{

}

void next_generation( void )
{

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
