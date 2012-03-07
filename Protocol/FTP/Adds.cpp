#include "Adds.h"
#include <iostream>


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

int FileExists(string file)
{
    struct stat buffer;
    int ret = stat(file.c_str(), &buffer);
    return ret == 0 ? 0 : errno;
}
