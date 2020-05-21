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

char *kernelSRC =
	"char get( __global uchar *arr, uint x, uint y, uint width, uint height) {" \
	"	if (x < 0 || y < 0 || x >= width || y >= height) return 0xFF;" \
	"	else return arr[y * width + x];" \
	"}" \
	"" \
	"__kernel void process(__global uchar *A, __global uchar *B, uint width, uint height) {" \
	"	uint index = get_global_id(0);" \
	"	uint y = (uint)(index / width);" \
	"	uint x = (uint)(index - y*height);" \
	"	char count = 0;" \
	"	" \
	"	if (!get(A, x-1, y-1, width, height)) count++;" \
	"	if (!get(A, x-1, y, width, height)) count++;" \
	"	if (!get(A, x-1, y+1, width, height)) count++;" \
	"	if (!get(A, x, y-1, width, height)) count++;" \
	"	if (!get(A, x, y+1, width, height)) count++;" \
	"	if (!get(A, x+1, y-1, width, height)) count++;" \
	"	if (!get(A, x+1, y, width, height)) count++;" \
	"	if (!get(A, x+1, y+1, width, height)) count++;" \
	"	" \
	"	if (count == 3) B[index] = 0x00;" \
	"	else B[index] = 0xFF;" \
	"}";

static uchar8_t count_neighbours ( uchar8_t **arr,
								   uint32_t x,
								   uint32_t y,
								   uint32_t width,
								   uint32_t height );
static int32_t gen_nextCL ( gen_t **generation );
static int32_t gen_setCL ( gen_t **generation );

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

	(*generation)->platformNums = 0;
	(*generation)->platformIds = NULL;
	(*generation)->deviceNums = 0;
	(*generation)->deviceIds = NULL;
	(*generation)->context = NULL;
	(*generation)->queue = NULL;
	(*generation)->program = NULL;
	(*generation)->kernelSrc = NULL;
	(*generation)->kernel = NULL;
	(*generation)->arrA = NULL;
	(*generation)->bufferA = NULL;
	(*generation)->arrB = NULL;
	(*generation)->bufferB = NULL;

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

	free( generation->arrA );
	free( generation->arrB );

	cl_int error;

	if ( !generation->queue )
		clFlush( generation->queue );

	if ( !generation->queue)
		clFinish( generation->queue );

	if ( !generation->deviceIds)
		clReleaseDevice( generation->deviceIds );

	if ( !generation->kernel)
		clReleaseKernel( generation->kernel );

	if ( !generation->program)
		clReleaseProgram( generation->program );

	if ( !generation->context)
		clReleaseContext( generation->context );

	if ( !generation->bufferA)
		clReleaseMemObject( generation->bufferA );

	if ( !generation->bufferB)
		clReleaseMemObject( generation->bufferB );

	if ( !generation->queue)
		clReleaseCommandQueue( generation->queue );

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

int gen_next ( gen_t **generation,
			   wcl cl)
{
	if ( !(*generation) )
		return 0xff;

	if ( cl == WITH_CL )
		return gen_nextCL ( generation );

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

static int32_t gen_nextCL ( gen_t **generation )
{
	if ( !(*generation) )
		return 0xff;

	if ( !(*generation)->platformIds ||
		 !(*generation)->deviceIds ||
		 !(*generation)->context ||
		 !(*generation)->queue ||
		 !(*generation)->program ||
		 !(*generation)->kernelSrc ||
		 !(*generation)->kernel ||
		 !(*generation)->bufferA ||
		 !(*generation)->bufferB )
	{
		uint32_t erro = GEN_SUCCESS;
		erro = gen_setCL( generation );
		if ( erro )
			return erro;
	}

	cl_int erro = CL_SUCCESS;
	size_t size = (size_t)(*generation)->width * (size_t)(*generation)->height;

	erro = clEnqueueWriteBuffer( (*generation)->queue,
								 (*generation)->bufferA,
								 CL_FALSE, 0,
								 sizeof(png_byte) *
										(*generation)->width *
										(*generation)->height,
								(*generation)->position ?
										(*generation)->arrB :
										(*generation)->arrA,
								0, NULL, NULL);
	if (erro)
		return erro;

	erro = clEnqueueWriteBuffer( (*generation)->queue,
								 (*generation)->bufferB,
								 CL_FALSE, 0,
								 sizeof(png_byte) * 
										(*generation)->width *
										(*generation)->height,
								 (*generation)->position ?
										(*generation)->arrA :
										(*generation)->arrB,
								 0, NULL, NULL);
	if (erro)
		return erro;

	size_t localWorkSize = 1;

	erro = clEnqueueNDRangeKernel ( (*generation)->queue,
									(*generation)->kernel,
									1, NULL,
									&size, &localWorkSize,
									0, NULL, NULL );
	if ( erro )
		return erro;

	erro = clEnqueueReadBuffer ( (*generation)->queue,
								 (*generation)->bufferA,
								 CL_TRUE, 0,
								 sizeof( png_byte ) * size,
								 (*generation)->arrA,
								 0, NULL, NULL );
	if ( erro )
		return erro;

	{
		size_t x, y;
		for ( x = 0; x < (*generation)->width; x++ )
			for ( y = 0; y < (*generation)->height; y++ )
				(*generation)->arrayA[y][x] =
						(*generation)->arrA[y * (*generation)->width + x];
	}


	erro = clEnqueueReadBuffer ( (*generation)->queue,
								 (*generation)->bufferB,
								 CL_TRUE, 0,
								 sizeof( png_byte ) * size,
								 (*generation)->arrB,
								 0, NULL, NULL );
	if ( erro )
		return erro;

	{
		size_t x, y;
		for ( x = 0; x < (*generation)->width; x++ )
			for ( y = 0; y < (*generation)->height; y++ )
				(*generation)->arrayB[y][x] =
				(*generation)->arrB[y * (*generation)->width + x];
	}

	(*generation)->position++;

	return GEN_SUCCESS;
}

static int32_t gen_setCL ( gen_t **generation )
{
	if ( !(*generation) )
		return 0xf0;
	
	cl_int erro = CL_SUCCESS;
	uint32_t i;
	char name[1024] = { '\0' };

	erro = clGetPlatformIDs ( 0, NULL, &(*generation)->platformNums );
	if ( erro || !(*generation)->platformNums )
		return erro;

	(*generation)->platformIds = (cl_platform_id*)malloc( sizeof ( cl_platform_id ) * (*generation)->platformNums );
	erro = clGetPlatformIDs ( (*generation)->platformNums, (*generation)->platformIds, NULL );
	if ( erro )
		return erro;

	for ( i = 0; i < (*generation)->platformNums; i++ )
	{
		printf ( "Platform:\t%u \n", i );

		erro = clGetPlatformInfo ( (*generation)->platformIds[i], CL_PLATFORM_NAME, 1024, &name, NULL );
		printf ( "Name:\t\t%s \n", erro ? "null" : name );

		erro = clGetPlatformInfo ( (*generation)->platformIds[i], CL_PLATFORM_VENDOR, 1024, &name, NULL );
		printf ( "Vendor:\t\t%s \n\n", erro ? "null" : name );
	}

	cl_int platform = 0;
	printf ( "Choose the platform: " );
	scanf ( "%d", &platform );
	if ( platform > (*generation)->platformNums )
		platform = 0;
	printf ( "-------------------------------------\n\n" );

	erro = clGetDeviceIDs( (*generation)->platformIds[platform],
						   CL_DEVICE_TYPE_ALL, 0, NULL,
						   &(*generation)->deviceNums );
	if ( erro || !(*generation)->deviceNums )
		return erro;

	(*generation)->deviceIds = (cl_device_id*)malloc ( sizeof ( cl_device_id ) * (*generation)->deviceNums );
	erro = clGetDeviceIDs ( (*generation)->platformIds[platform],
							CL_DEVICE_TYPE_ALL,
							(*generation)->deviceNums,
							(*generation)->deviceIds,
							&(*generation)->deviceNums );
	if ( erro )
		return erro;

	for ( i = 0; i < (*generation)->deviceNums; ++i )
	{
		printf ( "Device:\t\t%u \n", i );

		erro = clGetDeviceInfo ( (*generation)->deviceIds[i], CL_DEVICE_NAME, 1024, &name, NULL );
		printf ( "Name:\t\t%s \n", erro ? "null" : name );

		erro = clGetDeviceInfo ( (*generation)->deviceIds[i], CL_DEVICE_VENDOR, 1024, &name, NULL );
		printf ( "Vendor:\t\t%s \n", erro ? "null" : name );

		erro = clGetDeviceInfo ( (*generation)->deviceIds[i], CL_DEVICE_VERSION, 1024, &name, NULL );
		printf ( "Version:\t%s \n\n", erro ? "null" : name );
	}

	cl_int device = 0;
	printf ( "Choose the platform: " );
	scanf ( "%d", &device );
	if ( device > (*generation)->deviceNums )
		device = 0;
	printf("-------------------------------------\n\n");
	
	(*generation)->context = clCreateContext ( NULL, (*generation)->deviceNums,
											   (*generation)->deviceIds,
											   NULL, NULL, &erro );
	if ( erro )
		return erro;
	
	(*generation)->queue = clCreateCommandQueue ( (*generation)->context, (*generation)->deviceIds[device], 0, &erro );
	if ( erro )
		return erro;
	
	{
		(*generation)->arrA = (png_byte*)malloc( sizeof( png_byte ) *
												(*generation)->width *
												(*generation)->height);

		size_t x, y;
		for ( x = 0; x < (*generation)->width; x++ )
			for ( y = 0; y < (*generation)->height; y++ )
				(*generation)->arrA[y * (*generation)->width + x] =
						(*generation)->arrayA[y][x];
	}

	(*generation)->bufferA = clCreateBuffer ( (*generation)->context,
											  CL_MEM_READ_WRITE,
											  sizeof( png_byte ) * 
													(size_t)(*generation)->width * 
													(size_t)(*generation)->height,
											  NULL,
											  &erro );
	if ( erro )
		return erro;

	{
		(*generation)->arrB = (png_byte*)malloc( sizeof( png_byte ) *
												 (*generation)->width *
												 (*generation)->height);

		size_t x, y;
		for ( x = 0; x < (*generation)->width; x++ )
			for ( y = 0; y < (*generation)->height; y++ )
				(*generation)->arrB[y * (*generation)->width + x] =
						(*generation)->arrayB[y][x];
	}

	(*generation)->bufferB = clCreateBuffer ( (*generation)->context,
											  CL_MEM_READ_WRITE,
											  sizeof(png_byte) *
													(*generation)->width *
													(*generation)->height,
											  NULL,
											  &erro);
	if ( erro )
		return erro;

	(*generation)->kernelSrc = kernelSRC;
	size_t kernelSrcLength = strlen ( (*generation)->kernelSrc );

	(*generation)->program = clCreateProgramWithSource ( (*generation)->context, 1,
														 (const char**)&(*generation)->kernelSrc,
														 (const size_t*)&kernelSrcLength,
														 &erro );
	if ( erro )
		return erro;

	erro = clBuildProgram ( (*generation)->program,
							(*generation)->deviceNums,
							(*generation)->deviceIds,
							NULL, NULL, NULL );
	if ( erro )
		return erro;

	(*generation)->kernel = clCreateKernel ( (*generation)->program, "process", &erro );
	if ( erro )
		return erro;

	erro = clSetKernelArg ( (*generation)->kernel, 0, sizeof(cl_mem), (void*)&(*generation)->bufferA );
	if ( erro )
		return erro;

	erro = clSetKernelArg ( (*generation)->kernel, 1, sizeof(cl_mem), (void*)&(*generation)->bufferB );
	if ( erro )
		return erro;
	
	erro = clSetKernelArg ( (*generation)->kernel, 2, sizeof(png_uint_32), (void*)&(*generation)->width );
	if ( erro )
		return erro;
	
	erro = clSetKernelArg ( (*generation)->kernel, 3, sizeof(png_uint_32), (void*)&(*generation)->height );
	if (erro)
		return erro;

	return GEN_SUCCESS;
}
