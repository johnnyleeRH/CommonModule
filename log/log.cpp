#include "log.hpp"

std::ofstream outputfile;

static const char *monthList[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static pthread_mutex_t logMutex =  PTHREAD_MUTEX_INITIALIZER;

void Log( const char* mode, const char* file, int line, const char* func, const char* fmt, ... ) {
    pthread_mutex_lock( &logMutex );
    const int bufLen = 8196;
    static char tbuf[32];
    time_t t = time(NULL);
    struct tm now;
    localtime_r( &t, &now );
    snprintf( tbuf, 32, "[%s %02d %02d:%02d:%02d] %s: ",
    monthList[now.tm_mon], now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, mode );
    va_list argList;
    static char cbuffer[bufLen];
    va_start( argList, fmt );
    vsnprintf( cbuffer, bufLen, fmt, argList );
    va_end( argList );
    outputfile << tbuf << " " << file << " [" << line << "] " << func << "(): " << cbuffer << std::endl;
    pthread_mutex_unlock( &logMutex );
}

void hexLog(const char* file, int line, const char* func,const void* src,int src_len) {
    pthread_mutex_lock( &logMutex );
    static char tbuf[32];	// time
    time_t t = time(NULL);
    struct tm now;
    localtime_r( &t, &now );
    snprintf( tbuf, 32, "[%s %02d %02d:%02d:%02d] %s: ",
              monthList[now.tm_mon], now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, "HEX" );
    std::cout << tbuf << " " << file << " [" << line << "] " << func << "(): ";
    for(int i =0 ;i < src_len ;++i)
    {
        printf("%02hhX",((const char *)src)[i]);
    }
    printf("\n");
    pthread_mutex_unlock( &logMutex );
}