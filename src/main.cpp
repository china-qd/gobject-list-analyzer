#include "utils.h"
#include <iostream>
#include <string.h>
#include "uiactions.h"

using namespace std;

GtkWidget *g_main_window = NULL;
GThread *g_uiThread = NULL;
Json::Value g_config;

GHashTable *g_ht_main = NULL;
gboolean g_enteruiloop = false;

void releaseObjects()
{

}

gboolean bExited = FALSE;
void exit(gboolean normally)
{
    if(bExited)
        return;
    bExited = TRUE;
    try {
        // LOG_INFO("gobject list analyzer exited %s.", normally?"normally":"unexcepted");
        if(normally) {
            LOG_INFO("gobject list analyzer exited %s.", normally?"normally":"unexcepted");
        } else {
            LOG_WARN("gobject list analyzer exited %s.", normally?"normally":"unexcepted");
        }
        if(normally)
            releaseObjects();
        closeSLog();
    } catch (...) {
        std::cout<<"error occured at function exit."<<endl;
    }
}

void exit() __attribute__((destructor));
void exit() {
    exit(false);
}

gboolean gtk_main_window_close(GtkWidget *widget, GdkEvent  *event, gpointer   user_data)
{
    static gboolean closed = false;
    if(closed)
        return false;
    closed = true;
    LOG_DEBUG("close main window action");
    gtk_widget_hide(g_main_window);
    ste::Utils::exitSystem();
    return true;
}

#ifdef NocVdiVer
#define MY_VERSION noc_str_value(NocVdiVer)
#else
#define MY_VERSION "Not built from our GIT!"
#endif

int just_show_version(int argc, char *argv[])
{
    if (2!= argc)
    {
        return 0;
    }

    if ( strcmp( "--version", argv[1]) )
    {
        return 0;
    }

    printf("Noc TC login window, version: %s\n", MY_VERSION  );
    return 1;

}


/*---------------- Main Function -------------------------------------------*/
int main (int argc, char *argv[])
{
    GError *error = NULL;
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    int should_full_screen = 0;

    if (just_show_version(argc,argv))
    {
        exit(true);
        return 0;
    }

    std::string cmdline;
    for(int i = 0; i < argc; i++)
    {
        cmdline += argv[i];

        //TODO: 需要更严肃的 cmd line opt parser 和 --help 支持
        if ( !strcmp("-f" , argv[i])  )
        {
            should_full_screen = 1;
        }
    }
    LOG_SYSINFO("gobject list analyzer start with command line: %s", cmdline.c_str());
    //atexit(exit);
    g_uiThread = g_thread_self();
/*---------------- Load tropolink.ini & init log ------------------------------*/
    ste::FileUtils::reload_config("resources/config/setup.cfg", g_config);
    LOG_INFO("load config status: %s.", g_config["parse_success"].asInt()?"success":"failure");
    if(!initSLog()) {
        LOG_SYSERROR("init slog failed!");
        return 1;
    }
    g_log_set_default_handler(gtkLogHandler, NULL);
    gtk_init(&argc, &argv);
/*---------------- Load Main(login) UI ----------------------------------------*/
    g_ht_main = g_hash_table_new(g_str_hash, g_str_equal);
    g_main_window = ste::GtkUtils::loadUIandGetWidget("gobject-list-analyzer.glade", g_ht_main, "window_main", NULL);

    g_signal_connect(g_main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(g_main_window, "delete-event", G_CALLBACK(gtk_main_window_close), NULL);

/*---------------- Load CSS ---------------------------------------------------*/
    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen,
                       GTK_STYLE_PROVIDER(provider),
                       GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gsize bytes_written, bytes_read;
    const gchar* home = "resources/css/main.css";
    gtk_css_provider_load_from_path (provider,
                                    g_filename_to_utf8(home, strlen(home), &bytes_read, &bytes_written, &error),
                                    NULL);
    if(error)
    {
        LOG_GERROR("load main.css", error);
    }
    g_object_unref (provider);

/*---------------- Show window and call msg cycle------------------------------*/
#ifndef G_OS_WIN32
    should_full_screen = 0;
#endif
    // we are a humble window in windows
    if ( should_full_screen )
    {
        gtk_window_fullscreen(GTK_WINDOW(g_main_window));
    }

    gtk_widget_show_all(g_main_window);
    g_enteruiloop = true;
    gtk_main();
    g_enteruiloop = false;
    releaseObjects();

    exit(true);
    return 0;
}
