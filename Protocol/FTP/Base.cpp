#include "Base.h"

struct MapStringComparsionStruct
{
    bool operator()  (const string a, const string b) const
    {
        return a.compare(b) < 0;
    }
};

std::map<string, FTPStates, MapStringComparsionStruct> commandTable;

void InitMessages();
void InitCommandTable();
void LoadConfigs();

void Init()
{
    configMgr = new ConfigMgr("ftp.conf", NULL, 0);
    protoLog = new SimpleLog(boolConfigs[CONFIG_BOOL_CONTROL], boolConfigs[CONFIG_BOOL_DEBUG], "Protocol.log");
    db = new Database();
    LoadConfigs();
    InitMessages();
    InitCommandTable();

    if (boolConfigs[CONFIG_BOOL_USE_DB_AUTH])
    {
        db->Connect(stringConfigs[CONFIG_STRING_DATABASE_HOST].c_str(), 
                    stringConfigs[CONFIG_STRING_DATABASE_USERNAME].c_str(), 
                    stringConfigs[CONFIG_STRING_DATABASE_PASSWORD].c_str(),  
                    stringConfigs[CONFIG_STRING_DATABASE_NAME].c_str());
        if (!db->IsConnected())
        {
            protoLog->outError("Cannot connect to Database !!!");
            app->Terminate();
        }
    }
}

void InitMessages()
{
    // Connect
    responseCodeMessage.insert(make_pair(220, "Welcome to Ikaros SMTP Server"));
    responseCodeMessage.insert(make_pair(231, "User logged out; service terminated."));
    responseCodeMessage.insert(make_pair(421, "Service not avaliable, closing control channel"));
    // Not Implemented
    responseCodeMessage.insert(make_pair(500, "Syntax error, command uncategorized"));
    responseCodeMessage.insert(make_pair(501, "Syntax error in parameters or arguments."));
    responseCodeMessage.insert(make_pair(502, "Not Implemented"));
    responseCodeMessage.insert(make_pair(530, "Not Logged In"));
    responseCodeMessage.insert(make_pair(550, "Requested Action not taken"));
    // Quit
    responseCodeMessage.insert(make_pair(221, "Bye"));

    // Login
    responseCodeMessage.insert(make_pair(331, "User name okay, need password."));
    responseCodeMessage.insert(make_pair(230, "User logged in, proceed."));

    // commands
    responseCodeMessage.insert(make_pair(200, "Ok Command successfull."));
    responseCodeMessage.insert(make_pair(227, "Entering Passive mode."));
    responseCodeMessage.insert(make_pair(250, "Requested file action okay, completed."));
    responseCodeMessage.insert(make_pair(257, "\"PATH\" created."));

    responseCodeMessage.insert(make_pair(150, "File status okay; about to open data connection."));
    responseCodeMessage.insert(make_pair(226, "Closing data connection. Requested file action successful."));
    responseCodeMessage.insert(make_pair(425, "Can't open data connection."));
    responseCodeMessage.insert(make_pair(426, "Connection closed; transfer aborted."));

    responseCodeMessage.insert(make_pair(450, "Requested file action not taken."));
    responseCodeMessage.insert(make_pair(550, "Requested action not taken."));
    protoLog->outControl("Initialised  %u Reponse Code Messages", responseCodeMessage.size());
}

void InitCommandTable()
{
    commandTable.insert(make_pair("user", FTP_LOGIN_USER));
    commandTable.insert(make_pair("pass", FTP_LOGIN_PASS));

    commandTable.insert(make_pair("cwd", FTP_CHANGE_WORKING_DIRECTORY));
    commandTable.insert(make_pair("pwd", FTP_PRINT_WORKING_DIRECTORY));
    commandTable.insert(make_pair("cdup", FTP_CHANGE_WORKING_DIRECTORY_TO_PARENT));
    commandTable.insert(make_pair("mkd", FTP_MAKE_DIR));
    commandTable.insert(make_pair("rmd", FTP_REMOVE_DIR));

    commandTable.insert(make_pair("port", FTP_SESSION_PORT));
    commandTable.insert(make_pair("pasv", FTP_SESSION_PASSIVE));
    commandTable.insert(make_pair("type", FTP_TRANSFER_TYPE));

    commandTable.insert(make_pair("quit", FTP_QUIT));
    commandTable.insert(make_pair("help", FTP_HELP));
    commandTable.insert(make_pair("noop", FTP_NOOP));
    commandTable.insert(make_pair("syst", FTP_SYST));

    commandTable.insert(make_pair("stor", FTP_STORAGE_STORE));
    commandTable.insert(make_pair("retr", FTP_STORAGE_RETRIEVE));
    commandTable.insert(make_pair("dele", FTP_STORAGE_DELETE));
    commandTable.insert(make_pair("list", FTP_LIST));
    protoLog->outControl("Initialised  %u Command definitions", commandTable.size());
}

void LoadConfigs()
{
    stringConfigs[CONFIG_STRING_GREETINGS] = configMgr->LoadStringConfig("Greetings", "Welcome to FTP Server");

    boolConfigs[CONFIG_BOOL_DEBUG] = configMgr->LoadBoolConfig("Debug", true);
    boolConfigs[CONFIG_BOOL_CONTROL] = configMgr->LoadBoolConfig("Control", true);

    intConfigs[CONFIG_INT_MAX_DATA_SEGMENT_SIZE] = configMgr->LoadIntConfig("MaxSegmentSize", 40000);
    intConfigs[CONFIG_INT_SEND_ERROR_WAIT_TIME] = configMgr->LoadIntConfig("SendErrorWaitTime", 20);
    intConfigs[CONFIG_INT_SEND_WAIT_TIME] = configMgr->LoadIntConfig("SendWaitTime", 0);

    intConfigs[CONFIG_INT_RECV_ERROR_WAIT_TIME] = configMgr->LoadIntConfig("RecvErrorWaitTime", 20);
    intConfigs[CONFIG_INT_RECV_WAIT_TIME] = configMgr->LoadIntConfig("RecvWaitTime", 0);

    boolConfigs[CONFIG_BOOL_USE_DB_AUTH] = configMgr->LoadBoolConfig("UseDBAuth", true);
    boolConfigs[CONFIG_BOOL_ALLOW_GUESTS] = configMgr->LoadBoolConfig("AllowGuests", true);

    stringConfigs[CONFIG_STRING_DATABASE_HOST] = configMgr->LoadStringConfig("Database.Host", "localhost");
    if (!stringConfigs[CONFIG_STRING_DATABASE_HOST].length())
        stringConfigs[CONFIG_STRING_DATABASE_HOST] = "localhost";

    stringConfigs[CONFIG_STRING_DATABASE_USERNAME] = configMgr->LoadStringConfig("Database.Username", "root");
    stringConfigs[CONFIG_STRING_DATABASE_PASSWORD] = configMgr->LoadStringConfig("Database.Password", "0");
    stringConfigs[CONFIG_STRING_DATABASE_NAME] = configMgr->LoadStringConfig("Database.Name", "ikaros");

    stringConfigs[CONFIG_STRING_ROOT_DIRECTORY] = configMgr->LoadStringConfig("RootDirectory", ".");
    if (!DirectoryInfo::IsDir(stringConfigs[CONFIG_STRING_ROOT_DIRECTORY]))
        stringConfigs[CONFIG_STRING_ROOT_DIRECTORY] = ".";

    protoLog->outDebug("CONFIG_STRING_GREETINGS= '%s'", stringConfigs[CONFIG_STRING_GREETINGS].c_str());
    protoLog->outDebug("CONFIG_INT_MAX_DATA_SEGMENT_SIZE= %d", intConfigs[CONFIG_INT_MAX_DATA_SEGMENT_SIZE]);
    protoLog->outDebug("CONFIG_INT_SEND_WAIT_TIME= %d", intConfigs[CONFIG_INT_SEND_WAIT_TIME]);
    protoLog->outDebug("CONFIG_INT_SEND_ERROR_WAIT_TIME= %d", intConfigs[CONFIG_INT_SEND_ERROR_WAIT_TIME]);
    protoLog->outDebug("CONFIG_INT_RECV_WAIT_TIME= %d", intConfigs[CONFIG_INT_RECV_WAIT_TIME]);
    protoLog->outDebug("CONFIG_INT_RECV_ERROR_WAIT_TIME= %d", intConfigs[CONFIG_INT_RECV_ERROR_WAIT_TIME]);
    protoLog->outDebug();
    protoLog->outDebug("CONFIG_BOOL_ALLOW_GUESTS= %s", toString(boolConfigs[CONFIG_BOOL_ALLOW_GUESTS]));
    protoLog->outDebug("CONFIG_STRING_ROOT_DIRECTORY= '%s'", stringConfigs[CONFIG_STRING_ROOT_DIRECTORY].c_str());
    protoLog->outDebug("CONFIG_BOOL_USE_DB_AUTH= %s", toString(boolConfigs[CONFIG_BOOL_USE_DB_AUTH]));
    protoLog->outDebug();
    protoLog->outDebug("CONFIG_STRING_DATABASE_HOST= '%s'", stringConfigs[CONFIG_STRING_DATABASE_HOST].c_str());
    protoLog->outDebug("CONFIG_STRING_DATABASE_USERNAME= '%s'", stringConfigs[CONFIG_STRING_DATABASE_USERNAME].c_str());
    protoLog->outDebug("CONFIG_STRING_DATABASE_PASSWORD= '%s'", stringConfigs[CONFIG_STRING_DATABASE_PASSWORD].c_str());
    protoLog->outDebug("CONFIG_STRING_DATABASE_NAME= '%s'", stringConfigs[CONFIG_STRING_DATABASE_NAME].c_str());
    protoLog->outDebug();
}

void processConnect(in_addr address , int fd)
{
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    FTP::SendGreetings(sock);
    protoLog->outControl("Recieved FTP Connection from:  %s", inet_ntoa(address));

    SessionDataStruct& session = sessionData[fd];
    session.homeDirectory = stringConfigs[CONFIG_STRING_ROOT_DIRECTORY];
}


void processDisconnect(in_addr /* address */, int fd)
{
    if (sessionStatus.find(fd) != sessionStatus.end())
        sessionStatus.erase(fd);
    if (sessionData.find(fd) != sessionData.end())
        sessionData.erase(fd);
    protoLog->outControl("Connection form %d  closed", fd);
}

void processRecieve(in_addr /* address */, int fd, char* data)
{
    Socket* sock = app->socketMgr->GetSocketByFd(fd);
    stringstream dat(data);

    SessionDataStruct& session = sessionData[fd];

    string strdata(data);
    strdata = trim(data);

    string command; // Command is four character string
    dat >> command;

    if (!(command[0] >='a' && command[0] <= 'z') && !(command[0] >='A' && command[0] <= 'Z')) // Invalid command
        return;

    FTPStates processCommand = FTP_UNKNOWN;

    if (commandTable.find(lower(command)) != commandTable.end())
        processCommand = commandTable[lower(command)];

    protoLog->outControl("Recieved Command:  %s Procesing Command:  %d", command.c_str(), processCommand);
    switch (processCommand)
    {
        case FTP_LOGIN_USER:
        {
            if (session.loginPrompted)
                return;
            string user;
            dat >> user;
            if (!user.length())
            {
                FTP::SendCommandResponse(sock, 500);
                return;
            }
            session.username = user;
            session.loginPrompted = true;
            FTP::SendCommandResponse(sock, 331); // Require Password
            break;
        }
        case FTP_LOGIN_PASS:
        {
            if (!session.loginPrompted)
                return;
            string password;
            dat >> password;
            if (lower(session.username).compare("anonymous") == 0 && boolConfigs[CONFIG_BOOL_ALLOW_GUESTS])
            {
                session.loginPrompted = false;
                session.loggedIn = true;
                FTP::SendCommandResponse(sock, 230); // Authed
                break;
            }
            if (!password.length())
            {
                session.loginPrompted = false;
                FTP::SendCommandResponse(sock, 500);
                return;
            }
            session.password = password;
            if (boolConfigs[CONFIG_BOOL_USE_DB_AUTH])
            {
                QueryResult* result = db->PQuery("Select * from users where username = '%s' and password = '%s'", 
                                                session.username.c_str(), session.password.c_str());
                if (!result)
                {
                    session.loginPrompted = false;
                    FTP::SendCommandResponse(sock, 530); // Not authed
                    break;
                }
                Field* fields = result->FetchFields();

                protoLog->outDebug("FTP:: User: %s  pass: '%s' Id: %d  logged In (root dir: '%s' )", session.username.c_str(), session.password.c_str(), fields[0].GetUInt32(), fields[3].GetString());
                session.homeDirectory = fields[3].GetString();
            }
            else
                protoLog->outDebug("FTP:: User: %s  pass: '%s' logged In", session.username.c_str(), session.password.c_str());
            session.loginPrompted = false;
            session.loggedIn = true;
            FTP::SendCommandResponse(sock, 230); // Authed
            break;
        }
        case FTP_PRINT_WORKING_DIRECTORY:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            FTP::SendSpecialCommandResponse(sock, 200, "'"+ session.actualDir +"' is actual directory.");
            break;
        }
        case FTP_TRANSFER_TYPE:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string type;
            dat >> type;
            type = type;
            if (lower(type)[0] == 'i')
            {
                session.transferType = TRANSFER_BINARY;
                FTP::SendCommandResponse(sock, 200); // ok
                break;
            }
            if (lower(type)[0] == 'a')
            {
                session.transferType = TRANSFER_ASCII;
                FTP::SendCommandResponse(sock, 200); // ok
                break;
            }
                FTP::SendCommandResponse(sock, 500); // error
            break;
        }
        case FTP_SESSION_PORT:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            int a[9], p = 0;
            memset (a, 0, 9 * sizeof (int));

            string addr(strdata.substr(strdata.find_first_of(' '), string::npos));
            string remaining = addr;

            while (remaining.find_first_of(',') != string::npos)
            {
                string part = remaining.substr(0, remaining.find_first_of(','));
                remaining = remaining.substr(remaining.find_first_of(',')+1, string::npos);
                stringstream(part) >> a[p++];
                if (remaining.find_first_of(',') == string::npos)
                    stringstream(remaining) >> a[p++];
            }

            for (uint8 i = 0; i < 4; i++)
                session.ip[i] = a[i];
            session.ip.Restore();
            //protoLog->outDebug("IP Struct:  %s  %d %d %d %d   0x%08x  0x%08x", 
            //session.ip.ToChar(), session.ip[0], session.ip[1],session.ip[2],session.ip[3], *(int*)(&session.ip.ipB), *(int*)(&session.ip.ipI.s_addr));
            session.port = a[4] * 256 + a[5];
            session.isPassive = false;

            protoLog->outDebug("Connect via address:  %s (%d.%d.%d.%d:%d)", addr.c_str(), a[0], a[1], a[2], a[3], a[4] * 256 + a[5]);
            FTP::SendCommandResponse(sock, 200); // ok
            break;
        }
        case FTP_CHANGE_WORKING_DIRECTORY_TO_PARENT:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string changeTo = session.actualDir.substr(0, session.actualDir.find_last_not_of('/'));
            changeTo = changeTo.substr(0, changeTo.find_last_of('/'));
            if (!changeTo.size())
                changeTo = '/';
            session.actualDir = changeTo;
            FTP::SendSpecialCommandResponse(sock, 250, "CDUP successfull");
            break;
        }
        case FTP_SESSION_PASSIVE:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            FTP::InitPassiveSock(session);
            protoLog->outDebug("Session %s IP:  %s  Port:  %hu", session.isPassive? "Passive" : "Active", session.ip.ToChar(), session.port);
            stringstream response;
            response << "Entering passive mode.";
            response << int(session.ip[0]) << ',' << int(session.ip[1]) << ',' << int(session.ip[2]) << ',' << int(session.ip[3]) << ',';
            response << int(session.port / 256) << ',' << int(session.port % 256);
            FTP::SendSpecialCommandResponse(sock, 227, response.str().c_str());
            break;
        }
        case FTP_LIST:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            if (!DirectoryInfo::IsDir(string(session.homeDirectory + session.actualDir).c_str()))
            {
                FTP::SendCommandResponse(sock, 550);
                break;
            }
            protoLog->outDebug("Listing Directory:   %s", string(session.homeDirectory + session.actualDir).c_str());
            TerminalFuction func("ls", "-la \"" + session.homeDirectory + session.actualDir + '"');
            char* output = func.RunWithCallback();
            FTP::SendOverDTP(fd, output, strlen(output));
            break;
        }
        case FTP_CHANGE_WORKING_DIRECTORY:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string dir = trim(dat.str().substr(dat.str().find_first_of(' '), string::npos));
            if (dir.compare(".") == 0)
            {
                if (!DirectoryInfo::IsDir(session.homeDirectory + session.actualDir))
                {
                    FTP::SendSpecialCommandResponse(sock, 550, "Directory '"+ session.actualDir +"' does not exists.");
                    break;
                }
                FTP::SendSpecialCommandResponse(sock, 200, "'"+ session.actualDir +"' is actual directory.");
                break;
            }
            if (dir[0] == '/')
            {
                session.actualDir = dir;
                if (session.actualDir[session.actualDir.length()-1] != '/')
                    session.actualDir += '/';
                if (!DirectoryInfo::IsDir(session.homeDirectory + session.actualDir))
                {
                    FTP::SendSpecialCommandResponse(sock, 550, "Directory '"+ session.actualDir +"' does not exists.");
                    break;
                }
                FTP::SendSpecialCommandResponse(sock, 200, "'"+ session.actualDir +"' is actual directory.");
                break;
            }
            if (dir.compare("..") == 0)
            {
                if (session.actualDir[session.actualDir.length()-1] == '/')
                    session.actualDir.erase(session.actualDir.length()-1, string::npos);
                session.actualDir.erase(session.actualDir.find_last_of('/'), string::npos);

                if (session.actualDir.length() == 0  || session.actualDir[session.actualDir.length()-1] != '/')
                    session.actualDir += '/';
                if (!DirectoryInfo::IsDir(session.homeDirectory + session.actualDir))
                {
                    FTP::SendSpecialCommandResponse(sock, 550, "Directory '"+ session.actualDir +"' does not exists.");
                    break;
                }
                FTP::SendSpecialCommandResponse(sock, 200, "'"+ session.actualDir +"' is actual directory.");
                break;
            }
            if (!DirectoryInfo::IsDir(session.homeDirectory + session.actualDir + dir))
            {
                FTP::SendSpecialCommandResponse(sock, 550, "Directory '"+ (session.actualDir + dir) +"' does not exists.");
                break;
            }
            session.actualDir += dir;
            if (session.actualDir[session.actualDir.length()-1] != '/')
                session.actualDir += '/';
            FTP::SendCommandResponse(sock, 250);
            break;
        }
        case FTP_NOOP:
            FTP::SendSpecialCommandResponse(sock, 200, "Noop");
        break;
        case FTP_SYST:
            FTP::SendSpecialCommandResponse(sock, 200, "Ikaros");
        break;
        case FTP_QUIT:
            FTP::SendCommandResponse(sock, 221);
            sock->Close();
            if (sessionStatus.find(fd) != sessionStatus.end())
                sessionStatus.erase(fd);
            if (sessionData.find(fd) != sessionData.end())
                sessionData.erase(fd);
            app->socketMgr->CloseSocketByFd(fd);
        break;
        case FTP_MAKE_DIR:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string dirname = trim(dat.str().substr(dat.str().find_first_of(' '), string::npos));
            string address = session.homeDirectory + session.actualDir + dirname;
            if (DirectoryEntry::CreateDirectory(address) == 0)
                FTP::SendSpecialCommandResponse(sock, 257, '"' + dirname + "\" created.");
            else
                FTP::SendCommandResponse(sock, 450); // Already Exists
            break;
        }
        case FTP_STORAGE_DELETE:
        case FTP_REMOVE_DIR:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string filename = trim(dat.str().substr(dat.str().find_first_of(' '), string::npos));
            string address = session.homeDirectory + session.actualDir + filename;
            protoLog->outDebug("Delete File/Directory Name: %s", address.c_str());
            if (filename[0] == '/')
                address= session.homeDirectory + filename;
            if (DirectoryEntry::RemoveEntry(address) == 0)
                FTP::SendCommandResponse(sock, 250);
            else
                FTP::SendCommandResponse(sock, 550); // Not Found
            break;
        }
        case FTP_STORAGE_STORE:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string filename = trim(dat.str().substr(dat.str().find_first_of(' '), string::npos));
            string address = session.homeDirectory + session.actualDir + filename;
            if (filename[0] == '/')
                address= session.homeDirectory + filename;
            protoLog->outDebug("Store File  FileName: %s", address.c_str());
            FTP::RecieveOverDTP(fd, address.c_str());
            break;
        }
        case FTP_STORAGE_RETRIEVE:
        {
            if (!session.loggedIn)
            {
                FTP::SendCommandResponse(sock, 530);
                return;
            }
            string filename = trim(dat.str().substr(dat.str().find_first_of(' '), string::npos));
            string address = session.homeDirectory + session.actualDir + filename;
            if (filename[0] == '/')
                address= session.homeDirectory + filename;
            protoLog->outDebug("Requesting File:  %s", address.c_str());
            if (FileExists(address) != 0)
            {
                FTP::SendCommandResponse(sock, 550); // Not Found
                break;
            }
            Files::BinFile file;
            char* tmp = (char*) file.readFile(address);
            if (!file.is_open())
            {
                FTP::SendCommandResponse(sock, 450); // Not Found 
                break;
            }
            char* output =  new char[file.getLength() + 1];
            memset(output, 0, file.getLength()+1);
            memcpy(output, tmp, file.getLength());
            protoLog->outDebug("Sending Requested file:  %s  length: %lu", address.c_str(), file.getLength());
            FTP::SendOverDTP(fd, output, file.getLength());
            break;
        }
        case FTP_HELP:
        case FTP_UNKNOWN:
            FTP::SendSpecialCommandResponse(sock, 502, "Command '"+ command +"' "+ responseCodeMessage[502]);
        break;
    }
}
