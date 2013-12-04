#include "gtkutils.h"
#include "utils.h"
#include <map>
#include <string>
#include <assert.h>
#include <fstream>

namespace ste {

GtkUtils::GtkUtils()
{
    //ctor
}

GtkUtils::~GtkUtils()
{
    //dtor
}

void GtkUtils::addWidgetToHashTable(gpointer data, gpointer widgetsHashTable)
{
    static GType gtBuildable = gtk_buildable_get_type();
    static GType gtWidget = gtk_widget_get_type();
    if(!g_type_check_instance_is_a((GTypeInstance*)data, gtBuildable))
        return;
    const gchar *name = gtk_buildable_get_name(GTK_BUILDABLE(data));
    if(name)
    {

        if(widgetsHashTable)
        {
            g_object_ref(data);
            g_hash_table_replace((GHashTable*)widgetsHashTable, g_strdup(name), data);
        }
        if(g_type_check_instance_is_a((GTypeInstance*)data, gtWidget)) {
            gtk_widget_set_name(GTK_WIDGET(data), g_strdup(name));
        }
    }
}
GtkWidget* GtkUtils::loadUIandGetWidget(const gchar* uiFilePath, GHashTable *widgetsHashTable, const gchar* windowName, const gchar* widgetName)
{
    static std::map<std::string ,std::string> file2contentMap;
    assert(NULL != uiFilePath);
    assert(NULL != windowName);
    GtkBuilder *builder;
    GtkWidget  *window, *widget=NULL;
    GError     *error = NULL;

    std::map<std::string ,std::string>::const_iterator item = file2contentMap.find(uiFilePath);
    if(item == file2contentMap.end()) {
        std::string strUIFilePath("resources/ui/");
        strUIFilePath += uiFilePath;
        std::ifstream in(strUIFilePath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
        if(!in.is_open()) {
            LOG_ERROR( "load ui file '%s' error", uiFilePath);
            SHOW_ERROR_THEN_EXIT("程序即将退出：程序文件出错！");
            return NULL;
        }
        int len = in.tellg();
        in.seekg (0, std::ios::beg);
        char *buffer = new char [len+1];
        in.read (buffer, len);
        buffer[len] = 0;
        in.close();
        file2contentMap[uiFilePath] = buffer;
        delete buffer;
    }
    builder = gtk_builder_new();
    //if( ! gtk_builder_add_from_file( builder, strUIFilePath.c_str(), &error ) )
    std::string fcontent = file2contentMap[uiFilePath];
    if(!gtk_builder_add_from_string(builder, fcontent.c_str(), fcontent.length(), &error))
    {
        LOG_ERROR( "parse ui file '%s' error: %s", uiFilePath, error->message );
        g_clear_error(&error);
        SHOW_ERROR_THEN_EXIT("程序即将退出：程序文件出错！");
        return NULL;
    }

    window = GTK_WIDGET( gtk_builder_get_object( builder, windowName ) );
    if(!window)
    {
        LOG_ERROR("can't find main window '%s' from ui file '%s'", windowName, uiFilePath);
        SHOW_ERROR_THEN_EXIT("程序即将退出：程序文件出错！");
        return NULL;
    }
    gtk_builder_connect_signals( builder, NULL );
    GSList* objectsList = gtk_builder_get_objects(builder);
    g_slist_foreach(objectsList, addWidgetToHashTable, widgetsHashTable);
    g_slist_free(objectsList);
    if(widgetName)
    {
        widget = GTK_WIDGET( gtk_builder_get_object( builder, widgetName ) );
        if(!widget)
        {
            LOG_ERROR( "can't find widget '%s' from ui file '%s'", widgetName, uiFilePath );
            SHOW_ERROR_THEN_EXIT("程序即将退出：程序文件出错！");
        }
        g_object_ref (widget);
        gtk_container_remove(GTK_CONTAINER(window), widget);
        gtk_widget_destroy(window);
    }
    else
    {
        widget = window;
    }

    g_object_unref( G_OBJECT( builder ) );
    return widget;
}
GtkWidget* GtkUtils::loadUIandGetAlignmentCenter(const gchar* uiFilePath, GHashTable *widgetsHashTable)
{
    return loadUIandGetWidget(uiFilePath, widgetsHashTable, "window_main", "alignment_center");
}

GtkWidget* GtkUtils::getWidget(const gchar* widgetName, GHashTable *widgetsHashTable)
{
    static GType gtWidget = gtk_widget_get_type();
    GtkWidget* wRet = NULL;
    if(widgetName) {
        if(widgetsHashTable) {
            gpointer data = g_hash_table_lookup(widgetsHashTable, widgetName);
            if(g_type_check_instance_is_a((GTypeInstance*)data, gtWidget))
                wRet = GTK_WIDGET(data);
        }
    }
    if(!wRet)
        LOG_WARN("could not find widget: %s", widgetName);
    return wRet;
}








}
