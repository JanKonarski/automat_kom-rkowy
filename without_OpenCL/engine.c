/*
 *
 * Autor: Jan Konarski
 * Autor: Maciek Ko³ek
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "engine.h"
#include "strs.h"

typedef unsigned char uchar8_t;

#define death (char)0xFF
#define alive (char)0x00



static uchar8_t count_neighbours ( uchar8_t **arr,
								   uint32_t x,
								   uint32_t y,
								   uint32_t width,
								   uint32_t height );

static int gen_create ( gen_t **generation,
				 uint32_t width,
				 uint32_t height )
{
	if ( (*generation) )
		return GEN_OBJECT_NOT_NULL;

	(*generation) = (gen_t*)malloc( sizeof( gen_t ));

	if ( !(*generation) )
		return GEN_CREATE_FAILURE;
	
	uint32_t i;
	uint32_t k;

	(*generation)->position = 0;

	(*generation)->width = width;
	(*generation)->height = height;
	(*generation)->png_ptr = NULL;
	(*generation)->info_ptr = NULL;

	(*generation)->arrayA = NULL;
	(*generation)->arrayB = NULL;

	(*generation)->arrayA = (png_bytep*)malloc( sizeof( png_bytep ) * height );
	(*generation)->arrayB = (png_bytep*)malloc( sizeof( png_bytep ) * height );
	if ( !(*generation)->arrayA || !(*generation)->arrayB )
		return 0xff;

	for (i = 0; i < height; i++)
	{
		(*generation)->arrayA[i] = (png_bytep)malloc( sizeof( png_byte ) * width );
		(*generation)->arrayB[i] = (png_bytep)malloc( sizeof( png_byte ) * width );

		for ( k = 0; k < width; k++ )
		{
			(*generation)->arrayA[i][k] = death;
			(*generation)->arrayB[i][k] = death;
		}
	}

	(*generation)->color_type = PNG_COLOR_TYPE_GRAY;
	(*generation)->bit_depth = 8;
	(*generation)->img_ptr = NULL;

	return GEN_SUCCESS;
}

int gen_destroy ( gen_t *generation )
{
	if ( !generation )
		return -0xff;

	uint32_t i;
	uint32_t k;

	for ( i = 0; i < generation->height; i++ )
	{
		free( generation->arrayA[i] );
		free( generation->arrayB[i] );
	}
	free( generation->arrayA );
	free( generation->arrayB );

	free( generation );

	return GEN_SUCCESS;
}

int gen_rand ( gen_t **generation,
			   uint32_t width,
			   uint32_t height )
{
	if ( (*generation) )
		return GEN_OBJECT_NOT_NULL;

	gen_create( generation, width, height );

	srand(time(NULL));
	int r = rand() % (width * height / 2) + 1;
	int i = 0;
	while (i++ < r)
		(*generation)->arrayA
				[rand() % height + 1]
				[rand() % width + 1] =
			alive;

	return GEN_SUCCESS;
}

int gen_load ( gen_t **generation,
			   char *fileName )
{
	if ( (*generation))
		return GEN_OBJECT_NOT_NULL;

	FILE* fp = fopen( fileName, "r" );
	if ( !fp )
		return 0xff;

	uint32_t width;
	uint32_t height;
	fscanf( fp, "%d %d\n", &width, &height );

	gen_create( &(*generation), width, height );

	int32_t x;
	int32_t y;
	while ( fscanf( fp, "%d %d\n", &x, &y ) == 2)
		(*generation)->arrayA[y][x] = alive;

	fclose( fp );

	return GEN_SUCCESS;
}

int gen_save ( gen_t *generation,
			   char *fileName )
{
	if ( !generation )
		return GEN_OBJECT_NOT_NULL;

	FILE* fp = fopen( fileName, "w" );
	if ( !fp )
		return 0xff;

	fprintf( fp, "%d %d \n", generation->width, generation->height );

	uint32_t x, y;
	for ( x = 0; x < generation->width; x++ )
		for ( y = 0; y < generation->height; y++ )
			if (( !generation->position ? generation->arrayA[y][x] : generation->arrayB[y][x] ) == alive )
				fprintf(fp, "%d %d \n", x, y);

	fclose( fp );

	return GEN_SUCCESS;
}

int gen_image ( gen_t *generation,
				char *fileName )
{
	if ( !generation )
		return 0xff;

	generation->img_ptr = fopen( fileName, "wb" );
	if ( !generation->img_ptr )
		return 0xfe;

	generation->png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	if ( !generation->png_ptr )
		return 0xfd;

	generation->info_ptr = png_create_info_struct( generation->png_ptr );
	if ( !generation->info_ptr )
		return 0xfc;

	if ( setjmp( png_jmpbuf( generation->png_ptr )))
		return 0xfb;

	png_init_io ( generation->png_ptr,
				  generation->img_ptr );

	if ( setjmp( png_jmpbuf( generation->png_ptr )))
		return 0xfa;

	png_set_IHDR( generation->png_ptr,
				  generation->info_ptr,
				  generation->width,
				  generation->height,
				  generation->bit_depth,
				  generation->color_type,
				  PNG_INTERLACE_NONE,
				  PNG_COMPRESSION_TYPE_BASE,
				  PNG_FILTER_TYPE_BASE );

	png_write_info ( generation->png_ptr,
					 generation->info_ptr );

	if ( setjmp( png_jmpbuf ( generation->png_ptr )))
		return 0xef;

	png_write_image ( generation->png_ptr,
					  !generation->position ? 
							generation->arrayA :
							generation->arrayB );

	png_write_image ( generation->png_ptr,
					  generation->arrayA );

	if ( setjmp( png_jmpbuf ( generation->png_ptr )))
		return 0xee;

	png_write_end ( generation->png_ptr, NULL );

    fclose ( generation->img_ptr );

	return GEN_SUCCESS;
}

int gen_next ( gen_t **generation)
{
	if ( !(*generation) )
		return 0xff;

	uint32_t x, y;
	for ( x = 0; x < (*generation)->width; x++ )
		for ( y = 0; y < (*generation)->height; y++ )
		{
			uchar8_t neighbours = count_neighbours( !(*generation)->position ?
														(*generation)->arrayA :
														(*generation)->arrayB,
													x,
													y,
													(*generation)->width,
													(*generation)->height );
			
			if ( (*generation)->position )
			{
				if ((*generation)->arrayB[y][x] == alive)
				{
					if (neighbours == 2 || neighbours == 3)
						(*generation)->arrayA[y][x] = alive;
					else
						(*generation)->arrayA[y][x] = death;
				}
				else
				{
					if (neighbours == 3)
						(*generation)->arrayA[y][x] = alive;
					else
						(*generation)->arrayA[y][x] = death;
				}
			}
				
			else
			{
				if ( (*generation)->arrayA[y][x] == alive )
				{
					if (neighbours == 2 || neighbours == 3)
						(*generation)->arrayB[y][x] = alive;
					else
						(*generation)->arrayB[y][x] = death;
				}
				else
				{
					if (neighbours == 3)
						(*generation)->arrayB[y][x] = alive;
					else
						(*generation)->arrayB[y][x] = death;
				}
			}
		}

	(*generation)->position++;

	return GEN_SUCCESS;
}

static uchar8_t count_neighbours( uchar8_t **arr,
								  uint32_t x,
								  uint32_t y,
								  uint32_t width,
								  uint32_t height )
{
	uchar8_t count = 0;

	uint32_t k, l;
	
	uint32_t kMin = (int32_t)(x - 1) > 0 ? (x - 1) : 0;
	uint32_t kMax = (int32_t)(x + 1) < width ? (x + 2) : (x + 1);

	uint32_t lMin = (int32_t)(y - 1) > 0 ? (y - 1) : 0;
	uint32_t lMax = (int32_t)(y + 1) < height ? (y + 2) : (y + 1);

	for ( k = kMin; k < kMax; k++ )
		for ( l = lMin; l < lMax; l++ )
			if ( k != x || l != y )
				if ( arr[l][k] == alive )
					count++;


	return count;
}
