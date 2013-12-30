#ifdef WIN32
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#else
#include <sys/time.h>
#endif

#include <time.h>
#include <stdarg.h>
#include <stdio.h>

#include "common/TLog.h"

/**
* Log time format
*/
#define FMT "%d-%b-%Y %H:%M:%S"

/**
* Message log file name
*/
static char *_logCommonFile = 0;

/**
* Error log file name
*/
static char *_logErrorFile = 0;

/**
* Message log stream
*/
static FILE *_logCommonStream = stdout;

/**
* Error log stream
*/
static FILE *_logErrorStream = stderr;

/**
* Log id
*/
static const char *_logIdent = 0;

void TLog::logIdent ( const char *ident ) {
	_logIdent = ident;
}

void TLog::logSetCommonStream ( FILE *f ) {
	// closing old log
	if ( _logCommonFile ) {
		fclose ( _logCommonStream );
	}
	_logCommonStream = f;
}

void TLog::logSetErrStream ( FILE *f ) {
	// closing old log
	if ( _logErrorFile ) {
		fclose ( _logErrorStream );
	}
	_logErrorStream = f;
}

void TLog::logSetErrorLog ( const char * file ) {
	FILE *f = fopen ( file, "a" );
	if ( !f ) {
		logErr ( "Cant open error log '%s'\n", file );
	} else {
		_logErrorStream = f;
	}
}

void TLog::logSetCommonLog ( const char * file ) {
	FILE *f = fopen ( file, "a" );
	if ( !f ) {
		logErr ( "Cant open common log '%s'\n", file );
	} else {
		_logCommonStream = f;
	}
}

void TLog::logMsg ( const char *m, ... ) {
	va_list ap;
#ifdef LOW_STACK
	char *p = new char[MAX_LOG_LEN];
#else
	char p[MAX_LOG_LEN];
#endif
	time_t t = time ( 0 );
	char buf[64];

#ifdef TARGET_UNIX
	struct tm tt;
	localtime_r ( &t, &tt );
	strftime ( buf, 64, FMT , &tt );
	va_start(ap, m);
	vsnprintf(p, MAX_LOG_LEN-1, m, ap);
	va_end(ap);
#else
	struct tm *tt;
	tt = localtime(&t);
	strftime ( buf, 64, FMT , tt );
	va_start(ap, m);
	_vsnprintf(p, MAX_LOG_LEN-1, m, ap);
	va_end(ap);
#endif
	
	if ( _logIdent ) {
		fprintf ( _logCommonStream, "[%s] (%s) msg: %s", buf, _logIdent, p );
	} else {
		fprintf ( _logCommonStream, "[%s] msg: %s", buf, p );  
	}
	
	fflush ( _logCommonStream );
#ifdef LOW_STACK
	delete p;
#endif
}

void TLog::logErr ( const char *m, ... ) {
	va_list ap;
#ifdef LOW_STACK
	char *p = new char[MAX_LOG_LEN];
#else
	char p[MAX_LOG_LEN];
#endif
	time_t t = time ( 0 );
	char buf[64];

#ifdef TARGET_UNIX
	struct tm tt;
	localtime_r ( &t, &tt );
	strftime ( buf, 64, FMT , &tt );
	va_start(ap, m);
	vsnprintf(p, MAX_LOG_LEN-1, m, ap);
	va_end(ap);
#else
	struct tm *tt;
	tt = localtime(&t);
	strftime ( buf, 64, FMT , tt );
	va_start(ap, m);
	_vsnprintf(p, MAX_LOG_LEN-1, m, ap);
	va_end(ap);
#endif
	if ( _logIdent ) {
		fprintf ( _logErrorStream, "[%s] (%s) err: %s", buf, _logIdent, p );  
	} else {
		fprintf ( _logErrorStream, "[%s] err: %s", buf, p );  
	}
	
	fflush ( _logErrorStream );
#ifdef LOW_STACK
	delete p;
#endif
}


// log automation close
class stupidOne {
public:
	stupidOne () {
	}

	~stupidOne () {
		if ( _logCommonFile ) {
			fclose ( _logCommonStream );
		}

		if ( _logErrorFile ) {
			fclose ( _logCommonStream );
		}
	}
};

// Pray this will start log before app init
static stupidOne logFileOpener;
