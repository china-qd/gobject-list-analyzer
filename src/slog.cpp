#include "slog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <glib.h>
#include <algorithm>
#ifdef G_OS_WIN32
    #include <windows.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <sys/time.h>
    #include <pthread.h>
    #define  CRITICAL_SECTION   pthread_mutex_t
    #define  _vsnprintf         vsnprintf
#endif
//SLog{
// max xize of one log file: 20M
#define MAXLOGSIZE (1<<20)
#define ARRSIZE(x) (sizeof(x)/sizeof(x[0]))
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
static int slog_inited = FALSE;
static SLogLevel slog_level = SL_LEVEL_DEBUG;
static CRITICAL_SECTION slog_locker;
static FILE *slog_file = NULL;

static char logfilename1[]="loging.log"; // current loging file
static char logfilename2[]="logged.log"; // old log
static char logstr[1<<12]; // 16k
static char datestr[16];
static char timestr[16];
static char mss[4];

#ifdef G_OS_WIN32
#define INITLOCK(x) InitializeCriticalSection(x)
#define DESTROYLOCK(x) DeleteCriticalSection(x)
#define LOCK(x) EnterCriticalSection(x)
#define UNLOCK(x) LeaveCriticalSection(x)
#else
#define INITLOCK(x) pthread_mutex_init(x,NULL)
#define DESTROYLOCK(x) pthread_mutex_destroy(x)
#define LOCK(x) pthread_mutex_lock(x)
#define UNLOCK(x) pthread_mutex_unlock(x)
#endif

// debug info warning error fatal
SLogLevel get_slog_level(char* pszLogLevel)
{
    SLogLevel lvlRet = slog_level;
    if(pszLogLevel) {
        std::string lvl(pszLogLevel);
        std::transform(lvl.begin(), lvl.end(), lvl.begin(), ::toupper);
        if(lvl.compare(0, 3, "DEB")) {
            lvlRet = SL_LEVEL_DEBUG;
        } else if(lvl.compare(0, 3, "INF")) {
            lvlRet = SL_LEVEL_INFO;
        } else if(lvl.compare(0, 3, "WAR")) {
            lvlRet = SL_LEVEL_WARN;
        } else if(lvl.compare(0, 3, "ERR")) {
            lvlRet = SL_LEVEL_ERROR;
        } else if(lvl.compare(0, 3, "FAT")) {
            lvlRet = SL_LEVEL_FATAL;
        }
    }
    return lvlRet;
}
void set_slog_level(SLogLevel level)
{
    slog_level = level;
}
int open_logfile()
{
    if(NULL==slog_file)
        slog_file=fopen(logfilename1,"a");
    return NULL!=slog_file;
}
int close_logfile()
{
    if(slog_file)
    {
        fclose(slog_file);
        slog_file = NULL;
    }
    return TRUE;
}
int change_log_file()
{
    close_logfile();
    remove(logfilename2);
    if (rename(logfilename1,logfilename2)) {
        slog_file=fopen(logfilename1,"a");
    }
    return TRUE;
}
int initSLog()
{
    if(slog_inited) return TRUE;
    INITLOCK(&slog_locker);
    close_logfile();
    if(!open_logfile()) {
        DESTROYLOCK(&slog_locker);
        return FALSE;
    }
    slog_inited = TRUE;
    return TRUE;
}
int closeSLog()
{
    if(!slog_inited) return TRUE;
    slog_inited = FALSE;
    DESTROYLOCK(&slog_locker);
    close_logfile();
    return TRUE;
}
void SLogV(SLogLevel level, const char* srcfile, int line_num, const char *pszFmt,va_list argp) {
    struct tm *now;
    struct timeb tb;
    const char* levelstr = "[*****]";
    switch(level) {
    case SL_LEVEL_DEBUG:
        levelstr = "[DEBUG]";
        break;
    case SL_LEVEL_INFO:
        levelstr = "[INFO] ";
        break;
    case SL_LEVEL_WARN:
        levelstr = "[WARN] ";
        break;
    case SL_LEVEL_ERROR:
        levelstr = "[ERROR]";
        break;
    case SL_LEVEL_FATAL:
        levelstr = "[FATAL]";
        break;
    default:
        break; //return;
    }
    if (NULL==pszFmt||0==pszFmt[0]) return;
    if (-1==_vsnprintf(logstr,ARRSIZE(logstr),pszFmt,argp)) logstr[ARRSIZE(logstr)-1]=0;
    ftime(&tb);
    now=localtime(&tb.time);
    sprintf(datestr,"%04d-%02d-%02d",now->tm_year+1900,now->tm_mon+1,now->tm_mday);
    sprintf(timestr,"%02d:%02d:%02d",now->tm_hour     ,now->tm_min  ,now->tm_sec );
    sprintf(mss,"%03d",tb.millitm);
    printf("%s %s.%s %s %s @%s#%d\n",datestr,timestr,mss,levelstr,logstr,srcfile,line_num);
    if (NULL!=slog_file) {
        fprintf(slog_file,"%s %s.%s %s %s @%s#%d\n",datestr,timestr,mss,levelstr,logstr,srcfile,line_num);
        fflush(slog_file);
        if (ftell(slog_file)>MAXLOGSIZE) {
            change_log_file();
            if (NULL==slog_file) return;
        }
    }
}
void SLog(SLogLevel level, const char* srcfile, int line_num, const char *pszFmt,...) {
    if((!slog_inited && !initSLog()) || level < slog_level)
        return;
    va_list argp;
    LOCK(&slog_locker);
    va_start(argp,pszFmt);
    SLogV(level, srcfile, line_num, pszFmt,argp);
    va_end(argp);
    UNLOCK(&slog_locker);
}
void SLogS(SLogLevel level, const char* srcfile, int line_num, const char *pszFmt,...) {
    if(level < slog_level || (!slog_inited && !open_logfile()))
        return;
    va_list argp;
    va_start(argp,pszFmt);
    SLogV(level, srcfile, line_num, pszFmt,argp);
    va_end(argp);
    if(!slog_inited) {
        close_logfile();
    }
}
//SLog}
