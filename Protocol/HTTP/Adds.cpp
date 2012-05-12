#include "Adds.h"

#include <iostream>

void FileAddress::Parse()
{
   // At the begining there was an file;

    size_t pos = file.rfind('/');
    if (pos == string::npos)
    {
        isValid = false;
        return; // We Have a Not Valid File Address Here
    }
    string Tfile = file.substr(pos+1, string::npos);
    path = file.substr(0, pos+1);
   // sLog->outString("FILE:   '%s'", file.c_str());
   // sLog->outString("TFILE:   '%s'", Tfile.c_str());
   // sLog->outString("PATH:   '%s'", path.c_str());
    pos = Tfile.find('?');
    if (pos == string::npos)
        fileName = Tfile;
    else
    {
        params = Tfile.substr(pos+1, string::npos);
        fileName = Tfile.substr(0, pos);
    }
   // sLog->outString("FILENAME:   '%s'", fileName.c_str());
   // sLog->outString("PARAMS:   '%s'", params.c_str());

    isValid = true;
}


bool isWithinRootDir(string file)
{
    int deep = 0;
  //  sLog->outString("::isWithinRootDir:   '%s'", file.c_str());
    if (file[0] == '/')
        file = file.substr(1, string::npos);
    while (file.find('/') != string::npos)
    {
        string test = file.substr(0, file.find('/'));
        if (!test.length() && deep == 0)
            return false; // Point on Root Directory
        if (!test.length())
            break;

        if (!test.compare("..")/* == 0*/)
            deep--;
        else
            deep++;
        file = file.substr(file.find('/'), string::npos);
    }
    if (deep < 0)
        return false;
    return true;
}

int FileExists(string file)
{
    struct stat buffer;
    int ret = stat(file.c_str(), &buffer);
    return ret == 0 ? 0 : errno;
}

string lower(string in)
{
    string out = in;
    for (uint32 i = 0; i< in.length();i++)
        if (out[i] >= 'A' && out[i] <= 'Z')
            out[i] = out[i] + 32;
    return out;
}

string trim(string in)
{
    size_t pos = in.find_first_not_of(" \r\n\t");
    in.erase(0, pos );
    pos = in.find_last_not_of(" \r\n\t");
    if (pos == string::npos)
        return in;
    in.erase(pos + 1, string::npos);
    return in;
}

string toString(size_t i)
{
    stringstream s;
    s << i;
    return s.str();
}
