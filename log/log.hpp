#ifndef SSEMD_LOG_HPP
#define SSEMD_LOG_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdarg>
#include <sstream>

extern std::ofstream outputfile;

void Log(const char *mode, const char *file, int line, const char *func, const char *fmt, ...);
void hexLog(const char* file,int line,const char* func,const void* src, int src_len);

#ifdef NO_TRACE
#define TRACE( fmt, ... )
#else
#define TRACE(fmt, ...) Log( "TRC", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__ )
#endif

#ifdef NO_WARN
#define WARN( fmt, ... )
#else
#define WARN(fmt, ...) Log( "WRN", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__ )
#endif

#ifdef NO_INFO
#define INFO( fmt, ... )
#else
#define INFO(fmt, ...) Log( "INF", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__ )
#endif

#ifdef NO_ERROR
#define ERROR( fmt, ... )
#define FATAL( fmt, ... )
#else
#define ERROR(fmt, ...) Log( "ERR", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__ )
#define FATAL(fmt, ...) Log( "FTL", __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__ )
#endif

#ifdef NO_HEX_PRINT
#define HEX_PRINT( fmt, ... )
//#define HEX_PRINT(src,len) xele_trade::xele_hexLog( __FILE__, __LINE__, __FUNCTION__,src,len)
#else
#define HEX_PRINT(src,len) Log( __FILE__, __LINE__, __FUNCTION__,src,len)
#endif

#endif	//SSEMD_LOG_HPP
