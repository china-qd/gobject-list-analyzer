#include "fileutils.h"
#include "utils.h"
#include <fstream>

namespace ste {

FileUtils::FileUtils()
{
    //ctor
}

FileUtils::~FileUtils()
{
    //dtor
}

gboolean FileUtils::isFileExists(std::string filename)
{
	std::ifstream cfgfile(filename.c_str());
	if(!cfgfile.is_open()) {
		return false;
	}
	cfgfile.close();
	return true;
}

int FileUtils::reload_config(const char *configFile, Json::Value& docRoot)
{
    docRoot["parse_success"] = 0;
    if(!isFileExists(configFile))
    {
        LOG_WARN("config file not exist.");
        return FALSE;
    }
    Json::Reader reader;
    std::ifstream config_doc(configFile);
    bool parsingSuccessful = reader.parse(config_doc, docRoot);

    if (!parsingSuccessful) {
        LOG_WARN("Failed to parse config Json, %s\n", reader.getFormattedErrorMessages().c_str()) ;
        return FALSE;
    }
    docRoot["parse_success"] = 1;
    return TRUE;
}

int FileUtils::save_config(const char *configFile, Json::Value& docRoot)
{
	std::string strCfg = docRoot.toStyledString();
	std::fstream f(configFile, std::ios_base::out);
	if(!f.is_open()) {
        LOG_ERROR("can't open config file '%s' for write", configFile);
        return FALSE;
	}
	f<<strCfg;
	f.flush();
	f.close();
	return TRUE;
}







}
