/*
 *
 * Autor: Jan Konarski
 *
 */

#include "strs.h"

#include <stdlib.h>

char *strjoin( const char *str1, const char *str2 )
{
    size_t length = strlen( str1 ) + strlen( str2 ) + 1;
    char *str = (char*)malloc( sizeof( char ) * length );

    size_t i = 0;
    while ( *str1 != '\0' )
    {
        str[i] = *str1;
        ++str1;
        ++i;
    }
    while ( *str2 != '\0' )
    {
        str[i] = *str2;
        ++str2;
        ++i;
    }
    str[i] = '\0';

    return str;
}
