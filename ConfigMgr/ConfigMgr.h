#include "Includes.h"
#include "Utils.h"
#include "DataArrays/file.h"

using std::string;
using std::map;
using std::make_pair;
using std::stringstream;
using Files::AsciiFile;

#ifndef __ConfigMgr
#define __ConfigMgr

class ConfigMgr
{
    public:
            typedef std::map<string, string, StringComparsionObject> Options;
            ConfigMgr(const char* file, char* argv[], int argc);
            ~ConfigMgr() { }

            int LoadIntConfig(string ConfigName, int Default = 0);
            string LoadStringConfig(string ConfigName, string Default = "");
            bool LoadBoolConfig(string ConfigName, bool Default = false);

            int LoadIntRunConfig(string ConfigName, string ShortFormat, int Default = 0);
            string LoadStringRunConfig(string ConfigName, string ShortFormat, string Default = "");
            bool LoadBoolRunConfig(string ConfigName, string ShortFormat, bool Default = false);

            bool RunPropertyExist(string ConfigName, string ShortFormat);
            bool FilePropertyExist(string ConfigName);

            Options FileOptions;
            Options RunOptions;
};

#endif // __ConfigMgr
