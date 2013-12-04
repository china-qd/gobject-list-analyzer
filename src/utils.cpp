#include "utils.h"

// useage: g_log_set_default_handler(gLogHandler, NULL);
void gtkLogHandler(const gchar *log_domain,
             GLogLevelFlags log_level,
             const gchar *message,
             gpointer user_data)
{
    int iLevel = log_level;
    iLevel &= ~G_LOG_FLAG_FATAL;
    iLevel &= ~G_LOG_FLAG_RECURSION;
    switch(iLevel) {
    case G_LOG_LEVEL_WARNING:
        LOG_WARN("[%s WARN]%s", log_domain, message);
        break;
    case G_LOG_LEVEL_CRITICAL:
        LOG_ERROR("[%s CRITICAL]%s", log_domain, message);
        break;
    case G_LOG_LEVEL_ERROR:
        LOG_ERROR("[%s ERROR]%s", log_domain, message);
        break;
    }
}


namespace ste {

Utils::Utils()
{
    //ctor
}

Utils::~Utils()
{
    //dtor
}

void Utils::exitSystem()
{
    NOTIMPLEMETED
}




















}
