#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <string>
#include <gtk/gtk.h>
#include <json/json.h>

namespace ste {

class FileUtils
{
    public:
        FileUtils();
        virtual ~FileUtils();
    protected:
    private:

    public:
        static gboolean isFileExists(std::string filename);
        static int reload_config(const char *configFile, Json::Value& docRoot);
        static int save_config(const char *configFile, Json::Value& docRoot);
};




}
#endif // FILEUTILS_H
