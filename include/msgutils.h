#ifndef MSGUTILS_H
#define MSGUTILS_H
#include <gtk/gtk.h>
namespace ste {
/**************************************
  init: setDefaultWindow
**************************************/

class MsgUtils
{
    public:
        MsgUtils();
        virtual ~MsgUtils();
    protected:
    private:
        static GtkWindow* defaultWindow;
    public:
        static void setDefaultWindow(GtkWindow* defaultWindow);
        static GtkWindow* getDefaultWindow();
        static void showMessageBox(GtkWindow *parent, GtkMessageType type, const char *srcfile, int line_num, const char *pszFmt,...)
        __attribute__ ((format (printf, 5, 6)));
        static int showConfirmBox(GtkWindow *parent, const char *srcfile, int line_num, const char *pszFmt,...)
        __attribute__ ((format (printf, 4, 5)));
};


}
#endif // MSGUTILS_H
