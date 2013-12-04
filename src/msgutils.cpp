#include "msgutils.h"
#include "gtkutils.h"
namespace ste {

void vsnprintf_t(char *buf, int bufsize, const char *pszFmt, va_list argp)
{
    #ifdef G_OS_WIN32
        if (-1==_vsnprintf(buf,bufsize,pszFmt,argp)) buf[bufsize-1]=0;
    #else
        if (-1==vsnprintf(buf,bufsize,pszFmt,argp)) buf[bufsize-1]=0;
    #endif // G_OS_WIN32
}

MsgUtils::MsgUtils()
{
    //ctor
}

MsgUtils::~MsgUtils()
{
    //dtor
}

GtkWindow* MsgUtils::defaultWindow = NULL;
void MsgUtils::setDefaultWindow(GtkWindow* defaultWindow)
{
    MsgUtils::defaultWindow = defaultWindow;
}
GtkWindow* MsgUtils::getDefaultWindow()
{
    return MsgUtils::defaultWindow;
}

void MsgUtils::showMessageBox(GtkWindow *parent, GtkMessageType type, const char *srcfile, int line_num, const char *pszFmt,...)
{
    static int BUFSIZE = 1<<10;
    char msgbuf[BUFSIZE];
    va_list argp;
    va_start(argp,pszFmt);
    vsnprintf_t(msgbuf, BUFSIZE, pszFmt, argp);
    va_end(argp);

    // msgbox.glade should add string to exe??
    GtkMessageDialog *msgDialog = GTK_MESSAGE_DIALOG(GtkUtils::loadUIandGetWidget("msgbox.glade", NULL, "message_Info", NULL));
    gtk_message_dialog_format_secondary_text(msgDialog, msgbuf);
    if(!parent) parent = getDefaultWindow();
    gtk_window_set_transient_for(GTK_WINDOW(msgDialog), parent);
    gtk_dialog_run(GTK_DIALOG(msgDialog));
    gtk_widget_destroy(GTK_WIDGET(msgDialog));
}
int MsgUtils::showConfirmBox(GtkWindow *parent, const char *srcfile, int line_num, const char *pszFmt,...)
{
    static int BUFSIZE = 1<<10;
    char msgbuf[BUFSIZE];
    va_list argp;
    va_start(argp,pszFmt);
    vsnprintf_t(msgbuf, BUFSIZE, pszFmt, argp);
    va_end(argp);
    GtkMessageDialog *msgDialog = GTK_MESSAGE_DIALOG(GtkUtils::loadUIandGetWidget("msgbox.glade", NULL, "message_confirm", NULL));
    gtk_message_dialog_format_secondary_text(msgDialog, msgbuf);
    if(!parent) parent = getDefaultWindow();
    gtk_window_set_transient_for(GTK_WINDOW(msgDialog), parent);
    gint ret = gtk_dialog_run(GTK_DIALOG(msgDialog));
    gtk_widget_destroy(GTK_WIDGET(msgDialog));
    return ret==0;
}





}
