#include <fstream>
#include <cstdio>
#include <string.h>
#include <sstream>

#include "binData.h"

#ifndef __Files
#define __Files


#define __Debug

#ifdef __Debug 
#include <iostream>
using std::cout ;
using std::endl ;
#endif



namespace Files {

using namespace std;

class AsciiFile : public std::fstream 
{
    public: 
            AsciiFile() : length(0), content(""), FileName("") { }
            AsciiFile(const char* fileName) : length(0), content("") {_openFile(fileName);}
            AsciiFile(string fileName) : length(0), content("") { _openFile(fileName.c_str()); }
	        ~AsciiFile() { if(is_open())close(); }
		  
		    char* readFile(string File) { _openFile(File.c_str()); _readFile(); return const_cast<char *>(content.c_str()); }
	  	    char* readFile(const char* File) { _openFile(File); _readFile(); return const_cast<char *>(content.c_str()); }
	  	    char* readFile() { return const_cast<char *>(content.c_str()); }
	  	    string sreadFile(string File) { _openFile(File.c_str()); _readFile(); return content; }
	  	    string sreadFile(const char* File) { _openFile(File); _readFile(); return content; }  
		    string sreadFile() { return content ; }

		    void writeFile(string Content) { content = Content; }
	        void writeFile(char* Content) { content = string(Content); }
	    
	        void append(char* Content) { content += string(Content); }
	        void append(string Content) { content += Content; }

		    string getFileName() { return FileName; }
	    
		    size_t getLength();

            size_t size() const { return length; }

		    char* toCharArray() const { return const_cast<char *>(content.c_str()); }
		    string toString() const { return content ;} ;
		
		    operator string() const { return content ;}
		    operator char*() const { return const_cast<char *>(content.c_str()); }
		    operator bool() { return (is_open() ); }
		
		
            friend std::ostream& operator <<(std::ostream &os,const AsciiFile &obj){os<<obj.content;return os ;};
		
		    void SaveToFile();
		    void LoadFromFile(string File) { _openFile(File.c_str()); }	
		
	   	    void _openFile(const char* fileName);  
    protected: 
		    size_t length ;
		    string content ;
		    string FileName ;
		    void _readFile();
};

class BinFile : public std::fstream 
{
    public: 
            BinFile(): length(0), content(), FileName("") { };
            BinFile(const char* fileName): length(0), content() {_openFile(fileName); }
            BinFile(string fileName): length(0), content() { _openFile(fileName.c_str()); }
		    ~BinFile() { if(is_open()) close(); }

		    byte* readFile(string File) { _openFile(File.c_str()); _readFile(); return content; }
	  	    byte* readFile(const char* File) { _openFile(File); _readFile(); return content; }
	  	    byte* readFile(){ return content ; };

		    void writeFile(char* Content) { content.clear(); content << Content; }
	        void append(char* Content) { content << Content; }

	        void writeFile(byte* Content, size_t length) { content.clear(); content.write(Content,length); }
	        void writeFile(BinnaryData Content) { content.clear(); content.write(Content.getBuffer(), Content.size()); }
	        void append(byte* Content, size_t length) { content.append(Content,length); }

		    string getFileName() const { return FileName; }

		    size_t getLength();

		    byte* toByteArray() const { return content; }

		    void SaveToFile();
		    inline void SaveToFile(string file)
            {
                SaveToFile(file.c_str());
            }
		    void SaveToFile(const char* file);
		    void LoadFromFile(string File) { _openFile(File.c_str()); }	

	   	    void _openFile(const char* fileName);

    protected: 
		    size_t length ;
		    BinnaryData content ;
		    string FileName ;
		    void _readFile();
};

}
#endif
