#include "Includes.cpp"
#include "HTTP.cpp"
#include "Adds.cpp"





void Init()
{
    configMgr = new ConfigMgr("http.conf", NULL, 0);
    protoLog = new SimpleLog(boolConfigs[CONFIG_BOOL_CONTROL], boolConfigs[CONFIG_BOOL_DEBUG], "Protocol.log");
    LoadConfig();
    InitStrings();
    InitErrorPages();
}

void InitStrings()
{
    errorStrings.insert(std::make_pair(200, "OK"));
    errorStrings.insert(std::make_pair(404, "File Not Found"));
    errorStrings.insert(std::make_pair(403, "Access Forbidden"));
    errorStrings.insert(std::make_pair(500, "Internal Server Error"));
    protoLog->outDebug("Initiated Error Strings    Actual strings: %u", errorStrings.size());
}

void InitErrorPages()
{
    errorPages.insert(std::make_pair(404,
"<html>\n\
    <head>\n\
        <title>404. Not Found</title>\n\
    </head>\n\
    <body>\n\
    <br\\><br\\>\n\
        <div style=\"padding-left:100px\">\n\
            <h1>Page Not Found</h1>\n\
            <p>File you are requesting cannot be found on this server.</p>\n\
        </div>\n\
    </body>\n\
</html>\n"
));

    errorPages.insert(std::make_pair(403,
"<html>\n\
    <head>\n\
        <title>403. Access Forbidden</title>\n\
    </head>\n\
    <body>\n\
    <br\\><br\\>\n\
        <div style=\"padding-left:100px\">\n\
            <h1>Access Forbidden</h1>\n\
            <p>You don't have a rights to view this file.</p>\n\
        </div>\n\
    </body>\n\
</html>"));
    errorPages.insert(std::make_pair(500,
"<html>\n\
    <head>\n\
        <title>500. Internal Server Error</title>\n\
    </head>\n\
    <body>\n\
    <br\\><br\\>\n\
        <div style=\"padding-left:100px\">\n\
            <h1>Internal Server Error</h1>\n\
            <p>This Unknown error may occur from time to time.</p>\n\
        </div>\n\
    </body>\n\
</html>"));
    protoLog->outDebug("Initiated Error Pages  Actual pages: %u", errorPages.size());
}

void DebugConfig()
{
    protoLog->outDebug("CONFIG_BOOL_ALLOW_DEFAULT_INDEX= %s", toString(boolConfigs[CONFIG_BOOL_ALLOW_DEFAULT_INDEX]));
    protoLog->outDebug("CONFIG_BOOL_DEBUG= %s", toString(boolConfigs[CONFIG_BOOL_DEBUG]));
    protoLog->outDebug("CONFIG_BOOL_CONTROL= %s", toString(boolConfigs[CONFIG_BOOL_CONTROL]));

    protoLog->outDebug("CONFIG_STRING_DEFAULT_INDEX_FILE= '%s'", stringConfigs[CONFIG_STRING_DEFAULT_INDEX_FILE].c_str());
    protoLog->outDebug("CONFIG_STRING_HOSTNAME= '%s'", stringConfigs[CONFIG_STRING_HOSTNAME].c_str());
    protoLog->outDebug("CONFIG_STRING_ROOT_DIR= '%s'", stringConfigs[CONFIG_STRING_ROOT_DIR].c_str());

    protoLog->outDebug("CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE= '%d'", intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE]);
    protoLog->outDebug();
}
void LoadConfig()
{
    boolConfigs[CONFIG_BOOL_ALLOW_DEFAULT_INDEX] = configMgr->LoadBoolConfig("AllowIndexFile", true);
    boolConfigs[CONFIG_BOOL_DEBUG] = configMgr->LoadBoolConfig("Debug", true);
    boolConfigs[CONFIG_BOOL_CONTROL] = configMgr->LoadBoolConfig("Control", true);

    stringConfigs[CONFIG_STRING_DEFAULT_INDEX_FILE] = configMgr->LoadStringConfig("IndexFileName", "index.html");
    stringConfigs[CONFIG_STRING_HOSTNAME] = configMgr->LoadStringConfig("Hostname", "localhost");
    stringConfigs[CONFIG_STRING_ROOT_DIR] = configMgr->LoadStringConfig("RootDirectory", "./");

    intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE] = configMgr->LoadIntConfig("MaximalMainThreadFileSize", 40000);
    if (intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE] <= 0)
    {
        protoLog->outError("MaximalMainThreadFileSize must be > 0 (configured value: %d) setting back to default 40000",
        intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE]);
        intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE] = 40000;
    }

    if (stringConfigs[CONFIG_STRING_ROOT_DIR][stringConfigs[CONFIG_STRING_ROOT_DIR].length()-1] == '/')
        stringConfigs[CONFIG_STRING_ROOT_DIR].erase(stringConfigs[CONFIG_STRING_ROOT_DIR].length()-1, 1);

    DebugConfig();
}

void processRecieve(in_addr /* address */, int fd, char* data)
{

    Socket* sock = app->socketMgr->GetSocketByFd(fd);

    BinnaryData dat(data, strlen(data));
    HTTPHeader header;

    string method;
    dat >> method;

    if (lower(method).compare("get") == 0)
        header.Request.method = METHOD_GET;
    else
    if (lower(method).compare("post") == 0)
        header.Request.method = METHOD_POST;
    else
    if (lower(method).compare("head") == 0)
        header.Request.method = METHOD_HEAD;
    else
        return; // Unsupported Method

    string RequestedFile;
    dat >> RequestedFile;
    header.Request.RequestedURI = RequestedFile; //

    string version;
    dat >> version;
    string line;
    do
    {
        string line = trim(dat.getline());
        if (line.find(':') != string::npos)
        {
            string property = trim(line.substr(0, line.find(':')));
            string value = trim(line.substr(line.find(':')+1, string::npos));
            if (lower(property).compare("host") == 0)
                header.Host = value;
            else
            if (lower(property).compare("user-agent") == 0)
                header.UserAgent = value;
            else
                header.additions.insert(std::make_pair(lower(property), value));
           // sLog->outString("Property:  '%s'   Value:  '%s'", property.c_str(), value.c_str());
        }
        if (!line.length())
            break;
    } while (true);
    protoLog->outDebug("FD: %d  Requesting File:  %s", fd, RequestedFile.c_str());


    FileAddress file(RequestedFile);
    if  (file.fileName.length() == 0)
    {
        if (boolConfigs[CONFIG_BOOL_ALLOW_DEFAULT_INDEX])
            file.fileName = stringConfigs[CONFIG_STRING_DEFAULT_INDEX_FILE];
        else
        {
            protoLog->outControl("HTTP Error: 404 Requesting Default Page (Not Enabled)");
            HTTPResponse::SendErrorPage(sock, 404);
            return;
        }
    }

   // sLog->outString("Recieved Packet:\n%s",dat.TextLike().c_str());
   // sLog->outString("File:  %s is  %s", RequestedFile.c_str(), file.isValid? "valid":"invalid" );

   // sLog->outString("Recieved HTTP Request: Method: %s   File: %s  Path:  %s",  method.c_str(), file.fileName.c_str(), file.path.c_str());

    if (!isWithinRootDir(file.path + file.fileName)) // 403 Forbidden
    {
        HTTPResponse::SendErrorPage(app->socketMgr->GetSocketByFd(fd), 403);
        return;
    }
    int ret;
    switch (ret = FileExists(stringConfigs[CONFIG_STRING_ROOT_DIR] + file.path + file.fileName))
    {
        case 0: // All OK
        {
            switch (header.Request.method)
            {
                case METHOD_GET:
                case METHOD_POST:
                    HTTPResponse::SendDirectPage(sock, file.path + file.fileName);
                    break;
                case METHOD_HEAD:
                    HTTPResponse::SendHeadPage(sock, file.path + file.fileName);
                    break;
                case METHOD_PUT:
                default:
                    break;
            }
            break;
        }
        case EACCES:
        {
            protoLog->outControl("HTTP Error: 403 Requesting File:  %s", string(stringConfigs[CONFIG_STRING_ROOT_DIR] + file.path + file.fileName).c_str());
            HTTPResponse::SendErrorPage(sock, 403);
            break;
        }
        default:
            protoLog->outControl("HTTP Error: 404 Requesting File:  %s", string(stringConfigs[CONFIG_STRING_ROOT_DIR] + file.path + file.fileName).c_str());
            HTTPResponse::SendErrorPage(sock, 404);
            break;
    }

}
