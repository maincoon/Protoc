/**
* Log facilities
*/

#ifndef __M_CONLOG_H__
#define __M_CONLOG_H__

#include <stdio.h>

#define MAX_LOG_LEN 2048

// logging functions

class TLog {

public:
	static void logMsg ( const char *format, ... );
	static void logErr ( const char *format, ... );
	static void logIdent ( const char *name );

	// stream setup methods

	static void logSetCommonStream ( FILE *out );
	static void logSetErrStream ( FILE *out );
	static void logSetErrorLog ( const char *fname );
	static void logSetCommonLog ( const char *fname );
};

#endif
