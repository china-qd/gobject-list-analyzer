#ifndef GTKUTILS_H
#define GTKUTILS_H
#include <gtk/gtk.h>
namespace ste {
/**************************************
  ui path will add prefix: resources/ui/
**************************************/

class GtkUtils
{
    public:
        GtkUtils();
        virtual ~GtkUtils();
    protected:
    private:
        static void addWidgetToHashTable(gpointer data, gpointer widgetsHashTable);
    public:
        static GtkWidget* loadUIandGetWidget(const gchar* uiFilePath, GHashTable *widgetsHashTable, const gchar* windowName, const gchar* widgetName);
        /**
         * load window: "window_main", return widget: "alignment_center"
         */
        static GtkWidget* loadUIandGetAlignmentCenter(const gchar* uiFilePath, GHashTable *widgetsHashTable);
        static GtkWidget* getWidget(const gchar* widgetName, GHashTable *widgetsHashTable);
};

}

#endif // GTKUTILS_H
