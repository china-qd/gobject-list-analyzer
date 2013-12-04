#ifndef UTILS_H
#define UTILS_H
#include "gtkutils.h"
#include "msgutils.h"
#include "runtimeutils.h"
#include "fileutils.h"
#include <json/json.h>

#ifdef USESLOG
#include "slog.h"
#else

#endif

//---------------- Log ----------------
#ifdef USESLOG
#define LOG_DEBUG(pszFmt,args...) SLog(SL_LEVEL_DEBUG,__FILE__,__LINE__,pszFmt,##args)
#define LOG_INFO(pszFmt,args...) SLog(SL_LEVEL_INFO,__FILE__,__LINE__,pszFmt,##args)
#define LOG_WARN(pszFmt,args...) SLog(SL_LEVEL_WARN,__FILE__,__LINE__,pszFmt,##args)
#define LOG_ERROR(pszFmt,args...) SLog(SL_LEVEL_ERROR,__FILE__,__LINE__,pszFmt,##args)
#define LOG_FATAL(pszFmt,args...) SLog(SL_LEVEL_FATAL,__FILE__,__LINE__,pszFmt,##args)
// log when not init logger
#define LOG_SYSINFO(pszFmt,args...) SLogS(SL_LEVEL_INFO,__FILE__,__LINE__,pszFmt,##args)
#define LOG_SYSERROR(pszFmt,args...) SLogS(SL_LEVEL_ERROR,__FILE__,__LINE__,pszFmt,##args)

#else

#endif
#define LOG_GERROR(msg,pGError) if(pGError!=NULL){\
    LOG_ERROR("%s -> GError: code=%d,domain=%s,message=%s", msg, pGError->code,g_quark_to_string(pGError->domain),pGError->message);\
    g_clear_error(&pGError);\
} else {\
    LOG_ERROR("%s -> GError: NULL", msg);\
}
#define NOTIMPLEMETED LOG_WARN("this function not implemted");

//---------------- Message ----------------
#define EXIT() ste::Utils::exitSystem()
#define SHOW_INFO_MSGBOX(pszFmt,args...) ste::MsgUtils::showMessageBox(NULL,GTK_MESSAGE_INFO,__FILE__,__LINE__,pszFmt,##args)
#define SHOW_WARN_MSGBOX(pszFmt,args...) ste::MsgUtils::showMessageBox(NULL,GTK_MESSAGE_WARNING,__FILE__,__LINE__,pszFmt,##args)
#define SHOW_ERROR_MSGBOX(pszFmt,args...) ste::MsgUtils::showMessageBox(NULL,GTK_MESSAGE_ERROR,__FILE__,__LINE__,pszFmt,##args)
#define SHOW_ERROR_THEN_EXIT(pszFmt,args...) SHOW_ERROR_MSGBOX(pszFmt,##args);EXIT();
#define SHOW_CONFIRM_BOX(pszFmt,args...) ste::MsgUtils::showConfirmBox(NULL,__FILE__,__LINE__,pszFmt,##args)

//---------------- Widget ----------------
#define GET_ENTRY_TEXT(entryName,ht) gtk_entry_get_text(GTK_ENTRY(ste::GtkUtils::getWidget(entryName,ht)))
#define SET_ENTRY_TEXT(entryName,text,ht) gtk_entry_set_text(GTK_ENTRY(ste::GtkUtils::getWidget(entryName,ht)),text)
#define SET_LABEL_TEXT(labelName,text,ht) gtk_label_set_text(GTK_LABEL(ste::GtkUtils::getWidget(labelName,ht)),(text))


// useage: g_log_set_default_handler(gLogHandler, NULL);
void gtkLogHandler(const gchar *log_domain,
             GLogLevelFlags log_level,
             const gchar *message,
             gpointer user_data);

namespace ste {

class Utils
{
    public:
        Utils();
        virtual ~Utils();
    protected:
    private:

    public:
//        static setExitSystemFunc();
        static void exitSystem(); // will call a call back

};

}
#endif // UTILS_H
