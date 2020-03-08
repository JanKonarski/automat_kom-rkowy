/*
 *
 * Autor: Jan Konarski
 *
 */

#include "log.h"

const char *log_types[] = { "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[34m", "\x1b[0m" };
const char *log_communications[] = { "INFO", "WARNING", "ERROR", "DEBUG" };

void logs( logtype_t lt, const char *msg )
{
#ifdef LOG_ACTIVE

    time_t rawtime;
    time( &rawtime );
    struct tm *tm = localtime( &rawtime );

    printf( "[%02d:%02d:%02d %s%s%s]: %s \n", tm->tm_hour,
                                              tm->tm_min,
                                              tm->tm_sec,
                                              log_types[lt], log_communications[lt], log_types[4],
                                              msg);

#endif
}
