#ifndef SLOG_H_INCLUDED
#define SLOG_H_INCLUDED
#include <memory>

enum eSLogLevel {
    SL_LEVEL_DEBUG = 1,
    SL_LEVEL_INFO,
    SL_LEVEL_WARN,
    SL_LEVEL_ERROR,
    SL_LEVEL_FATAL
};
typedef enum eSLogLevel SLogLevel;
int initSLog();
int closeSLog();
SLogLevel get_slog_level(char* pszLogLevel=0);
void set_slog_level(SLogLevel level);
void SLog(SLogLevel level, const char *srcfile, int line_num, const char *pszFmt,...)
__attribute__ ((format (printf, 4, 5)));
void SLogS(SLogLevel level, const char* srcfile, int line_num, const char *pszFmt,...)
__attribute__ ((format (printf, 4, 5)));
#define SLOG_DEBUG(pszFmt,args...) SLog(SL_LEVEL_DEBUG,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_INFO(pszFmt,args...) SLog(SL_LEVEL_INFO,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_WARN(pszFmt,args...) SLog(SL_LEVEL_WARN,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_ERROR(pszFmt,args...) SLog(SL_LEVEL_ERROR,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_FATAL(pszFmt,args...) SLog(SL_LEVEL_FATAL,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_SYSINFO(pszFmt,args...) SLogS(SL_LEVEL_INFO,__FILE__,__LINE__,pszFmt,##args)
#define SLOG_SYSERROR(pszFmt,args...) SLogS(SL_LEVEL_ERROR,__FILE__,__LINE__,pszFmt,##args)

#endif // SLOG_H_INCLUDED
