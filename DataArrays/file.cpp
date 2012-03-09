#include "file.h"

namespace Files
{

void AsciiFile::_openFile(const char* fileName)
{
    if ((FileName != string(fileName)) && (string(fileName).length() > 0))
    {
        open(fileName);
        FileName = string(fileName);
    }
    if (!is_open())
        open(FileName.c_str());
}


void AsciiFile::_readFile()
{
    if (!is_open())
        return;
    if (length != getLength())
        length = getLength();
    if (!length)
        return;
    char* buffer = new char[length + 1];
    memset(buffer, 0, length + 1);
    read(buffer, length);
    content = string(buffer);
}

size_t AsciiFile::getLength()
{
     if (!is_open())
        return 0;
    seekg(0, ios_base::end);
    long len = tellg();
    seekg(0, ios_base::beg);
    return len;
}

void AsciiFile::SaveToFile()
{
    if (!is_open())
        return;
    seekp(ios_base::beg);
    write(content.c_str(), content.length());
}

void BinFile::_openFile(const char* fileName)
{
    if ((FileName.compare(fileName) != 0) && (strlen(fileName) > 0))
    {
        open(fileName);
        FileName = string(fileName);
    }
    if (!is_open())
        open(FileName.c_str());
}


void BinFile::_readFile()
{
    if (!is_open())
        return;
    if (length != getLength())
        length = getLength();
    if (!length)
        return;
    content.clear();
    content.reserve(length);
    read(content, length);
}

size_t BinFile::getLength()
{
    if (!is_open())
        return 0;
    seekg(0, ios_base::end);
    long len = tellg();
    seekg(0, ios_base::beg);
    return len;
}

void BinFile::SaveToFile()
{
    if (!is_open())
        return;
    seekp(ios_base::beg);
    write(content, content);
}

void BinFile::SaveToFile(const char* file)
{
    _openFile(file);
    if (!is_open())
        return;
    seekp(ios_base::beg);
    write(content, content);
}

}
