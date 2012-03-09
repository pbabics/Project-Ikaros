#include "ConfigMgr.h"


ConfigMgr::ConfigMgr(const char* conf, char* argv[], int argc)
{
    if (argc> 0)
        for (int i = 1 ; i < argc ; i++)
        {
            string arg = string(argv[i]);
            string property, value;
            refTrim(arg);
            if (arg.find('=') != string::npos)
            {
                property = arg.substr(arg.find_first_not_of('-'), arg.find('=') - arg.find_first_not_of('-'));
                refTrim(property);
                value =  arg.substr(arg.find('=') + 1 , string::npos);
                refTrim(value);
            }
            else
            {
                property = arg.substr(arg.find_first_not_of('-'), string::npos);
                value = string("1");
            }
            RunOptions.insert(make_pair(property, value));
        }

    if (conf)
    {
        AsciiFile config(conf);
        if (config)
        {
            char buff[1024];
            string line ;
            do
            {
                config.getline(buff, 1024);
                line = buff;
                if( (line.length() > 0) &&
                    (line[0] != '#') && (line[0] != ';') &&
                    (line.find('=') != string::npos))
                {
                    refTrim(line);
                    string property = line.substr(0, line.find('='));
                    refTrim(property);
                    string value =  line.substr(line.find('=') + 1, string::npos);
                    refTrim(value);
                    if (property.length() && value.length())
                        FileOptions.insert(make_pair(property, value));
                }
            } while (!config.eof());
        }
    }
}

int ConfigMgr::LoadIntConfig(string ConfigName, int Default)
{
    if (FileOptions.size() > 0)
    {
        if (FileOptions.find(ConfigName) != FileOptions.end())
        {
            stringstream s;
            int value = 0;
            s.str(FileOptions[ConfigName]);
            s >> value;
            return value;
        }
    }
    return Default;
}

string ConfigMgr::LoadStringConfig(string ConfigName, string Default)
{
    if (FileOptions.size() > 0)
        if (FileOptions.find(ConfigName) != FileOptions.end())
            return FileOptions[ConfigName];

    return Default;
}

bool ConfigMgr::LoadBoolConfig(string ConfigName, bool Default)
{
    if (FileOptions.size() > 0)
    {
        if (FileOptions.find(ConfigName) != FileOptions.end())
        {
            stringstream s;
            bool value = 0;
            s.str(FileOptions[ConfigName]);
            s >> value;
            return value;
        }
    }
    return Default;
}

int ConfigMgr::LoadIntRunConfig(string ConfigName, string ShortFormat, int Default)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
        {
            stringstream s;
            int value = 0;
            s.str(RunOptions[ConfigName]);
            s >> value;
            return value;
        }
        else
            if (RunOptions.find(ShortFormat) != RunOptions.end())
            {
                stringstream s;
                int value = 0;
                s.str(RunOptions[ShortFormat]);
                s >> value;
                return value;
            }
    }
    return Default;
}

string ConfigMgr::LoadStringRunConfig(string ConfigName, string ShortFormat, string Default)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
            return RunOptions[ConfigName];
        else if (RunOptions.find(ShortFormat) != RunOptions.end())
            return RunOptions[ShortFormat];
    }
    return Default;
}

bool ConfigMgr::LoadBoolRunConfig(string ConfigName, string ShortFormat, bool Default)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
        {
            stringstream s;
            bool value = false;
            s.str(RunOptions[ConfigName]);
            s >> value;
            return value;
        }
        else
        if (RunOptions.find(ShortFormat) != RunOptions.end())
        {
            stringstream s;
            bool value = false;
            s.str(RunOptions[ShortFormat]);
            s >> value;
            return value;
        }
    }
    return Default;
}

bool ConfigMgr::RunPropertyExist(string ConfigName, string ShortFormat)
{
    if (RunOptions.size() > 0)
    {
        if (RunOptions.find(ConfigName) != RunOptions.end())
            return true;
        else if (RunOptions.find(ShortFormat) != RunOptions.end())
            return true;
    }
    return false;
}

bool ConfigMgr::FilePropertyExist(string ConfigName)
{
    if (FileOptions.size() > 0)
        if (FileOptions.find(ConfigName) != RunOptions.end())
            return true;
    return false;
}
