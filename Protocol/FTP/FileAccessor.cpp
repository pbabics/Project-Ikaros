#include "FileAccessor.h"


FileNameVector const DirectoryInfo::GetFiles()
{
    if (!entryName)
        return FileNameVector();

    if (dir)
        return FileNameVector();

    if (files.size())
        return files;

    if (!dir)
        dir = opendir(entryName);

    if (!dir)
    {
        err = errno;
        if (err == ENOTDIR)
            isDir = false;
        return FileNameVector();
    }

    files.clear();
    struct dirent* dp = NULL;
    errno = 0;
    while ((dp = readdir(dir)) != NULL)
    {
        files.push_back(dp->d_name);
    }
    if (errno)
        err = errno;

    closedir(dir);
    return files;
}

bool DirectoryInfo::IsDir(const char* dirName)
{
    DIR* dir = opendir(dirName);

    if (!dir)
        return false;

    closedir(dir);
    return true;
}

int DirectoryEntry::Remove()
{
    int ret = 0;
    ret = remove(fileName);
    if (ret == 0)
        exists = false;
    else
    {
        err = errno;
        if (err == ENOENT)
            exists = false;
    }
    return ret;
}

int DirectoryEntry::Rename(char* newname)
{
    int ret = 0;
    ret = rename(fileName, newname);
    if (ret == 0)
        fileName = newname;
    else
    {
        err = errno;
        if (err == ENOENT)
            exists = false;
    }
    return ret;
}

