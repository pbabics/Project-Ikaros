#include "Includes.cpp"


#ifndef __Adds
#define __Adds

typedef std::multimap<string, string, compStr> Vars;

enum RequestMethod
{
    METHOD_GET = 1,
    METHOD_PUT,
    METHOD_POST,
    METHOD_HEAD
};

struct HTTPHeader
{
    struct HTTPRequest
    {
        RequestMethod method;
        string RequestedURI;
        Byte Version;
    } Request;
    string Host;
    string UserAgent;
    Vars additions;
};

struct FileAddress
{
    FileAddress(): isValid(false){ }
    FileAddress(string f): file(f), isValid(false) { Parse(); }
    string path;
    string fileName;
    string params;

    string file;
    void Parse();
    void Parse(string s) { file = s; Parse(); }
    bool isValid;
};

bool isWithinRootDir(string file);
int FileExists(string file);
string toString(size_t i);

#endif // __Adds
