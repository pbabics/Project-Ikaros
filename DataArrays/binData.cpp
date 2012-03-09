#include "binData.h"

/*                                       */
/*     Alocators / dealocators           */
/*                                       */

void BinnaryData::reserve(size_t len)
{
    if (maxLength == len)
        return;
    if (maxLength > len)
    {
        memset(data + len, 0, maxLength -len); 
        return;
    }
    if (maxLength < len)
    {
        byte *buffer = new byte[len]; 
        memset(buffer, 0, len);
        if (usedLength)
        {
            memcpy(buffer,data,usedLength);
            pget = buffer + (pget-data);
            pput = buffer + (pput-data);
            delete data;
        }
        maxLength = len;
        data = buffer;
        if (!pget || pget-data < 0 || (pget-data) > maxLength)pget = data;
        if (!pput || pput-data < 0 || (pput-data) > maxLength)pput = data;
    }
}

void BinnaryData::clear()
{
    if (usedLength)
    {
        memset(data, 0, usedLength);
        delete data;
    }
    _init();
}

/*                                       */
/*     IO pointers funcs                 */
/*                                       */


void BinnaryData::seekg(size_t to = 0, IOposition pos = beg)
{
    switch (pos)
        {
            case beg: pget = data + (to > maxLength ? usedLength : to);break;
            case end: pget = data + (to > usedLength ? usedLength : usedLength - to);break;    
        }
}
void BinnaryData::seekg(IOposition pos)
{
    switch (pos)
        {
            case beg: pget = data ;break;
            case end: pget = data + usedLength ;break;    
        }
}
void BinnaryData::seekg(int by)
{
    pget =  ((pget-data)-by > 0 ? pget-by : data+usedLength+((pget-data)-by));
}


void BinnaryData::seekp(size_t to = 0, IOposition pos = beg)
{
    switch (pos)
    {
        case beg: 
            pput = data + ( to > maxLength ? usedLength : to);
            break;
        case end: 
            pput = data + (to > usedLength ? usedLength : usedLength - to);
            break;    
    }
}

void BinnaryData::seekp(IOposition pos)
{
    switch (pos)
    {
        case beg: 
            pput = data;
            break;
        case end:
            pput = data + usedLength;
        break;    
    }
}

void BinnaryData::seekp(int by)
{
    pput =  ((pput-data)-by > 0 ? pput-by : data+usedLength+((pput-data)-by));
}

template <class T>
void BinnaryData::skip()
{
    pget =  ((pget-data)-sizeof(T) > 0 ? pget-sizeof(T) : data+usedLength+((pget-data)-sizeof(T)));
}

template <class T>
void BinnaryData::read(T& dat)
{
    memcpy(&dat, pget, sizeof(T));
    pget += sizeof(T);
}

template <class T>
void BinnaryData::write(T& dat)
{
    if ((pput - data + sizeof(T)) > usedLength)
        grow((pput - data + sizeof(T)) - usedLength);
    memcpy(pput, &dat, sizeof(T));
    pput += sizeof(T);
    usedLength += sizeof(T);
}

/*                                       */
/*    Output public member functions     */
/*                                       */


size_t BinnaryData::read(void *dest, size_t num= maxpos) const
{
    if (usedLength == 0 || !dest)
        return 0 ;
    memcpy(dest, data, (usedLength > num ? num : usedLength));
    return (usedLength>num?num:usedLength);
}

size_t BinnaryData::readsome(void *dest, size_t num = maxpos)
{
    assert(pget);
    if (usedLength == 0 || !dest)
        return 0;
    memcpy(dest, pget, (usedLength - (pget - data) > num ? num : usedLength - (pget - data)));
    pget = pget + (usedLength - (pget - data) > num ? num : usedLength - (pget - data));
    return (usedLength - (pget - data) > num ? num : usedLength - (pget - data));
}

/*                                       */
/*     Input public member functions     */
/*                                       */

void BinnaryData::write(const void *src, size_t num)
{
    if (num == 0 || !src)
        return;
    if ((num + usedLength) > maxLength)
        grow(num);
    memcpy(pput, src, num);
    usedLength += num ;
    pput += num ;
}

void BinnaryData::append(const void *src, size_t num)
{
    if (num == 0 || !src)
        return;
    if ((num + usedLength) > maxLength)
        grow(num);
    memcpy(data + usedLength, src, num);
    usedLength += num ;
}


/*                                       */
/*     Subscript Overload                */
/*                                       */

byte BinnaryData::operator [](size_t pos)const
{
    if (pos > maxLength)
        pos = maxLength; 
    return byte(*(data + pos));
}



/*                                       */
/*     Inserter Overloads   (normal)     */
/*                                       */


BinnaryData& BinnaryData::operator << (byte dat)
{
    write<byte>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (char dat)
{
    write<char>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (char* dat)
{
    size_t len = strlen(dat); //  -1 'cause character
    write(dat, len);
    return *this;
}


BinnaryData& BinnaryData::operator << (short dat)
{
    write<short>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (unsigned short dat)
{
    write<unsigned short>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (int dat)
{
    write<int>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (unsigned int dat)
{
    write<unsigned int>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (long dat)
{
    write<long>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (unsigned long dat)
{
    write<unsigned long>(dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (BinnaryData dat)
{
    write(dat.getBuffer(), dat);
    return *this;
}

BinnaryData& BinnaryData::operator << (std::string dat)
{
    write(dat.c_str(), dat.length());
    return *this;
}


BinnaryData& BinnaryData::operator << (std::stringstream dat)
{
    write(dat.str().c_str(), dat.str().length());
    return *this;
}

/*                                       */
/*     Inserter Overloads   (constants)  */
/*                                       */


BinnaryData& BinnaryData::operator << (const char* dat)
{
    size_t len = strlen(dat); //  -1 'cause character
    write(const_cast<char*>(dat),len);
    return *this;
}

/*                                       */
/*     Extractor Overloads               */
/*                                       */



BinnaryData&  BinnaryData::operator >> (byte& dat)
{
    read<byte>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (char& dat)
{
    read<char>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (short& dat)
{
    read<short>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (unsigned short& dat)
{
    read<unsigned short>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (int& dat)
{
    read<int>(dat);
    return *this;
}
BinnaryData&  BinnaryData::operator >> (unsigned int& dat)
{
    read<unsigned int>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (long& dat)
{
    read<long>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (unsigned long& dat)
{
    read<unsigned long>(dat);
    return *this;
}

BinnaryData&  BinnaryData::operator >> (std::string& dat)
{
    size_t pos = _find(' ');
    size_t endline = _find('\n');
    if (*(pget + endline) == '\r')
        endline--;
    if (pos > endline)
        pos = endline;
    char stream[pos + 1];
    memset(stream, 0, pos+1);
    readsome(stream ,pos);
    pget++;
    dat = string(stream);
    return *this;
}

string BinnaryData::getline()
{
    size_t pos = _find('\n');
    if (*(pget + pos) == '\r')
        pos--;
    char stream[pos+1];
    memset(stream, 0, pos+1);
    readsome(stream ,pos);
    if (*pget == '\r')
        pget++;
    if (*pget == '\n')
        pget++;
    return string(stream);
}

/*                                       */
/*     Searching / Locating / Finding    */
/*                                       */

size_t BinnaryData::find(byte ch)
{
    for (size_t i = 0;i<usedLength;i++)
        if (*(data+i) == ch)
            return i ;
    return maxpos ;
}

size_t BinnaryData::find(byte* ch, size_t length)
{
    for (size_t i = 0;i<usedLength-length;i++)
        if (memcmp(data+i,ch,length))
            return i ;
    return maxpos ;
}

size_t BinnaryData::_find(byte ch)
{
    for (size_t i = 0; i < usedLength - (pget-data); i++)
        if (*(pget+i) == ch)
            return i ;
    return maxpos ;
}

size_t BinnaryData::locate(byte ch)
{
    for (size_t i = pget-data;i<usedLength;i++)
        if (*(pget+i) == ch)
            return i ;
    return maxpos-(pget-data) ;
}

/*                                       */
/*     String Outputs                    */
/*                                       */

string BinnaryData::TextLike()
{
    return string((char*)data);
}

string BinnaryData::HexLike()
{
    stringstream n;
    char x[3];
    for (size_t i = 0; i < size(); i++)
        {
            memset(x, 0, 3);
            sprintf(x, "%02x",*(data + i));
            n << x;
            if (i < size()-1)
                {
                    if ((i % 8 == 0) && i > 0)
                        n << '\n';
                        else
                        n << ' ';
                }
        }
    return n.str();
}
