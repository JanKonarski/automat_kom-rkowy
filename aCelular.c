/*
 *
 * Autor: Jan Konarski
 * Autor: Maciek Ko³ek
 *
 */

#include "engine.h"
#include "strs.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

int main( int argc, char **argv )
{

	if ( argc == 1 )
		return EXIT_FAILURE;

	uint32_t width = 0;
	uint32_t height = 0;
	char *input_fileName = NULL;
	char *output_fileName = NULL;
	char *prefix_imageName = NULL;
	uint32_t loops = 0;
	wcl cl = WITHOUT_CL;
	bool gui = false;
	char* help =
		"\t-w (wight) <unsigned int> \n" \
		"\t-h (height) <unsigned int> \n" \
		"\t-i (input config) <char *> \n" \
		"\t-o (output config) <char *> \n" \
		"\t-p (image name prefix) <char *> \n" \
		"\t-l (loops) <unsigned int> \n" \
		"\t-c (OpenCL) [WITH_CL | WITHOUT_CL]\n";

	size_t i;
	for ( i = 1; i < argc; )
	{
		if ( strcmp ( argv[i], "-help" ) == 0 )
		{
			printf( "%s", help );
			return EXIT_FAILURE;
		}
		
		switch ( argv[i][1] )
		{
		case 'w':
			width = strtoul ( argv[++i], NULL, 0 );
			i++;
			break;

		case 'h':
			height = strtoul ( argv[++i], NULL, 0 );
			i++;
			break;

		case 'i':
			input_fileName = argv[++i];
			i++;
			break;

		case 'o':
			output_fileName = argv[++i];
			i++;
			break;

		case 'p':
			prefix_imageName = argv[++i];
			i++;
			break;

		case 'l':
			loops = strtoul ( argv[++i], NULL, 0 );
			i++;
			break;

		case 'c':
			cl = strcmp ( argv[++i], "WITH_CL" ) == 0 ? WITH_CL : WITHOUT_CL;
			i++;
			break;

		default:
			printf( "Invalid syntax \n" );
			return EXIT_FAILURE;
		}
	}

	if ( (width != 0 || height != 0) && input_fileName )
		return EXIT_FAILURE;

	int erro = GEN_SUCCESS;
	gen_t *gen = NULL;

	erro = input_fileName ? gen_load( &gen, input_fileName ) : gen_rand( &gen, width, height );
	if ( erro ) return erro;

	if ( output_fileName )
	{
		erro = gen_save( gen, output_fileName );
		if ( erro )	return erro;
	}


	
	erro = gen_image ( gen, strjoin(strjoin(prefix_imageName, strjoin("_", "0")), ".png") );
	if ( erro ) return erro;

	uint32_t k;
	char value[11];
	for ( k = 0; k < loops; k++ )
	{
		erro = gen_next ( &gen, cl ? WITHOUT_CL : WITH_CL );
		if ( erro ) return erro;

		sprintf( value, "%lu", k+1 );
		erro = gen_image ( gen, strjoin(strjoin(prefix_imageName, strjoin("_", value)), ".png") );
		if ( erro ) return erro;
	}

	erro = gen_destroy( gen );
	if ( erro ) return erro;

	getchar();
	
	return EXIT_SUCCESS;
}
