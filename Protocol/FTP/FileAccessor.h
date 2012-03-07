#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <string>
#include "file.h"

typedef std::vector<char*> FileNameVector;
typedef unsigned int uint32;
typedef unsigned long uint64;

class DirectoryEntryInfo
{
    public:
        DirectoryEntryInfo(char* _entryName, struct stat _fileStat): fileStat(_fileStat), entryName(_entryName) { }
        uint32 GetMode() const { return fileStat.st_mode; }
        uint32 GetGID() const { return fileStat.st_gid; }
        uint32 GetUID() const { return fileStat.st_uid; }
        uint64 GetSize() const { return fileStat.st_size; }
        time_t GetLastAccess() const { return fileStat.st_atime; }
        time_t GetLastModifycation() const { return fileStat.st_mtime; }
        time_t GetLastStatusChange() const { return fileStat.st_ctime; }
        char*  GetEntryName() const { return entryName; }


    protected:
        struct stat fileStat;
        char* entryName;
};

DirectoryEntryInfo const* GetFileInfo(char* fileName)
{
    struct stat fileStat;
    if (stat(fileName, &fileStat) == 0)
        return new DirectoryEntryInfo(fileName, fileStat);
    else
        return NULL;
}

class DirectoryInfo : public DirectoryEntryInfo
{
    public:
        DirectoryInfo(char* _directoryName, struct stat _dirStat): 
        DirectoryEntryInfo(_directoryName, _dirStat), dir(NULL), isDir(true), err(0) { }

        char* GetDirectoryName() const { return entryName; }
        FileNameVector const GetFiles();

        bool IsDirectory() const { return isDir; }
        uint32 GetError() const { return err; }
        static bool IsDir(const char* dirName);
        static bool IsDir(string dirName) { return DirectoryInfo::IsDir(dirName.c_str()); }
    protected:
        DIR* dir;
        FileNameVector files;
        bool isDir;
        uint32 err;
};

class DirectoryEntry // Used for manipulation with files and folders
{
    public:
        DirectoryEntry(const char* _fileName): fileName(_fileName), exists(true), err(0) { }
        DirectoryEntry(string _fileName): fileName(_fileName.c_str()), exists(true), err(0) { }
        int Rename(char* newname);
        int Remove();

        uint32 GetError() const { return err; }
        bool Exists() const { return exists; }
        const char* GetFileName() const { return fileName; }

        static int RenameEntry(const char* oldname, const char* newname) { return rename(oldname, newname); }
        static int RemoveEntry(const char* filename) { return remove(filename); }
        static int RemoveEntry(string filename) { return remove(filename.c_str()); }
        static int CreateDirectory(const char* dirname, uint32 mode = 0x01FF) { return mkdir(dirname, mode); }
        static int CreateDirectory(string dirname, uint32 mode = 0x01FF) { return mkdir(dirname.c_str(), mode); }
    protected:
        const char* fileName;
        bool exists;
        uint32 err;
};

class TerminalFuction // Used for manipulation output of applications ran from terminal USEABLE ONLY ON LINUX
{
    public:
        TerminalFuction(string _program, string _args): program(_program), args(_args) { }
        int Run() const { return system((program + " " + args).c_str()); }
        char* RunWithCallback(int* returnValue = NULL) const 
        {
            int ret = 0;
            stringstream call;
            call << program << ' ' << args << " > tmpfile.txt";
            //printf("\nCall:  %s\n",call.str().c_str());
            ret = system(call.str().c_str());
            if (returnValue)
                *returnValue = ret;
            Files::AsciiFile file;
            char* tmp = file.readFile("tmpfile.txt");
            char* output =  new char[strlen(tmp)+1];
            memset(output, 0, strlen(tmp)+1);
            memcpy(output, tmp, strlen(tmp));
            DirectoryEntry::RemoveEntry("tmpfile.txt");
            return output;
        }

    private:
        std::string program;
        std::string args;
};
