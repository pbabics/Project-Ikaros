#include <string> // string.h / cstring for memcpy , memset
#include <assert.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <sstream>

using namespace std;

#ifndef __BinnaryData
#define __BinnaryData

typedef unsigned char   byt ;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef unsigned long   uint64;

#define BinData BinnaryData

enum IOposition
{
    beg,
    end
};


class BinnaryData
{
    public:
        BinnaryData() { _init(); }
        BinnaryData(void *src, size_t num) { _init(); write(src,num); }
        ~BinnaryData() { if ((usedLength || maxLength) && data) delete data; }

        size_t read(void *dest, size_t num)const;
        size_t readsome(void *dest, size_t num);
        void write(const void *src, size_t num);
        void append(const void *src, size_t num);

        byte get() { byte g = *pget; (pget-data) == usedLength ? pget= data+usedLength : pget+=1 ; return g; }
        void unget() { (pget-data) > 0 ? pget-=1 : pget= data+usedLength ; }
        void put(byte dat) { write<byte>(dat); }

        byte* getBuffer() const { return data; }
        byte* getBufferPut() const { return pput; }

        size_t tellg() const { return (data-pget); }
        void seekg(size_t to, IOposition pos);
        void seekg(IOposition pos);
        void seekg(int by);

        size_t teelp() const { return (data-pput); }
        void seekp(size_t to, IOposition pos);
        void seekp(IOposition pos);
        void seekp(int by);

        byte operator [](size_t) const;
        operator int() const { return usedLength; }
        operator size_t() const { return usedLength; }
        operator long int() const { return usedLength; }
        operator char*() const { return (char*)(data); }
        operator byte*() const { return (data); }
        operator void*() const { return data; }
        operator bool() const { return (usedLength > 0 ? true : false); }

        BinnaryData& operator << (byte);
        BinnaryData& operator << (char);
        BinnaryData& operator << (char*);
        BinnaryData& operator << (const char*);
        BinnaryData& operator << (short);
        BinnaryData& operator << (unsigned short);
        BinnaryData& operator << (int);
        BinnaryData& operator << (unsigned int);
        BinnaryData& operator << (long);
        BinnaryData& operator << (unsigned long);
        BinnaryData& operator << (BinnaryData);
        BinnaryData& operator << (std::string);
        BinnaryData& operator << (std::stringstream);

        BinnaryData& operator >> (byte&) ;
        BinnaryData& operator >> (char &);
        BinnaryData& operator >> (int&); ;
        BinnaryData& operator >> (unsigned int&);
        BinnaryData& operator >> (short&);
        BinnaryData& operator >> (unsigned short&);
        BinnaryData& operator >> (long&);
        BinnaryData& operator >> (unsigned long&);
        BinnaryData& operator >> (std::string&);

        string getline();

        template <class T>
        void skip();

        template <class T>
        void write(T& data);

        template <class T>
        void read(T& data);

        string TextLike();
        string HexLike();

        size_t find(byte);
        size_t _find(byte);
        size_t find(byte*, size_t);
        size_t locate(byte ch);

        void setLength(size_t len) { usedLength = len; }

        void clear();
        size_t size() const { return usedLength; }
        size_t capacity() const { return maxLength; }
        void reserve(size_t);
        void grow(size_t by) { reserve(maxLength + by); }

        static const size_t maxpos = -1;

    private:
        byte  *data;
        size_t   usedLength;
        size_t   maxLength;
        byte  *pget;
        byte  *pput;

        void _init(){data= NULL ; pget = NULL ; pput = NULL; usedLength = 0 ; maxLength = 0;}
};

#endif
