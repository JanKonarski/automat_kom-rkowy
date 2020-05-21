/*
 *
 * Autor: Jan Konarski
 * Autor: Maciek Ko³ek
 *
 */

#include <stdint.h>
#include <png.h>

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#else
#include <CL/cl.h>
#endif

#ifndef _ENGINE_H_
#define _ENGINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Error Codes */
#define GEN_SUCCESS				0
#define GEN_OBJECT_NOT_NULL		-1
#define GEN_CREATE_FAILURE		-2
#define GEN_DESTROY_FAILURE		-3
#define GEN_READ_CONFIG_FAILURE	-4


	typedef enum { WITH_CL, WITHOUT_CL } wcl;

	typedef struct gen
	{

		unsigned char position : 1;

		png_uint_32 width;
		png_uint_32 height;
		png_byte color_type;
		png_byte bit_depth;

		png_structp png_ptr;
		png_infop info_ptr;
		png_bytep *arrayA;
		png_bytep *arrayB;
		png_FILE_p *img_ptr;

		cl_uint platformNums;
		cl_platform_id *platformIds;
		cl_uint deviceNums;
		cl_device_id *deviceIds;
		cl_context *context;
		cl_command_queue *queue;
		cl_program *program;
		char *kernelSrc;
		cl_kernel *kernel;
		unsigned char *arrA;
		cl_mem *bufferA;
		unsigned char *arrB;
		cl_mem *bufferB;

	} gen_t;

	int gen_destroy ( gen_t *generation );

	int gen_rand ( gen_t **generation,
				   uint32_t width,
				   uint32_t height );

	int gen_load ( gen_t **generation,
				   char *fileName );

	int gen_save ( gen_t *generation,
				   char *fileName );

	int gen_image ( gen_t *generation,
					char *imageName );

	int gen_next ( gen_t **generation,
				   wcl cl );


#ifdef __cplusplus
}
#endif

#endif	/* _ENGINE_3_H_ */
