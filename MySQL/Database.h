#include <mysql/mysql.h>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "Defs.h"

class MySQLDriver;
class Fields;
class Field;
class QueryResult;
class Database;


#define MAX_SQL_BUFFER 2048

class MySQLDriver
{
    public:
        static uint32 GetClientVersion() { return mysql_get_client_version(); }
        static const char* GetClientInfo() { return mysql_get_client_info(); }
        static void Init(int argc = 0, char **argv = NULL, char **groups = NULL) { mysql_library_init(argc, argv, groups); }
        static void End() { mysql_library_end(); }
};


class Field
{
    public:
        Field(): value(NULL) { }

        const char* GetString() { return value; }

        uint8 GetUInt8() { return atoi(value); }
        uint16 GetUInt16() { return atoi(value); }
        uint32 GetUInt32() { return atoi(value); }
        uint64 GetUInt64() { return atol(value); }

        char GetChar() { return atoi(value); }
        short GetShort() { return atoi(value); }
        int GetInt() { return atoi(value); }
        long GetLong() { return atol(value); }

        float GetFloat() { return atof(value); }
        double GetDouble() { return atof(value); }

        void SetStructureData(void *v, MYSQL_FIELD i)
        {
            value = reinterpret_cast<const char*>(v);
            info = i;
        }

        bool IsNumeric() const { return IS_NUM(info.flags); }
        bool IsNotNull() const { return info.flags & NOT_NULL_FLAG; }
        bool IsBlob() const { return IS_BLOB(info.flags); }
        bool IsUnsigned() const { return info.flags & UNSIGNED_FLAG; }
        bool IsPrimaryKey() const { return info.flags & PRI_KEY_FLAG; }
        bool HasDefaultValue() const { return info.flags & NO_DEFAULT_VALUE_FLAG; }

        char* GetFieldName() const { return info.name; }
        char* GetTableName() const { return info.table; }
        char* GetDatabaseName() const { return info.db; }

    private:
        const char* value;
        MYSQL_FIELD info;
};


class QueryResult
{
    public:
        QueryResult(MYSQL_RES* result, MYSQL_ROW row, MYSQL_FIELD* fields);
        ~QueryResult();

        uint32 TellRow() { return _currentRowId; }
        void SeekRow(uint64 rowId) { _currentRowId = rowId; mysql_data_seek(_result, rowId); NextRow(); }

        uint64 GetRowsCount() const { return num_rows; }
        uint32 GetFieldsCount() const { return num_fields; }

        Field* FetchFields() { return _currentRow; }

        bool NextRow();

    private:
        void FillFields(MYSQL_ROW row, MYSQL_FIELD* fields);

        uint64 _currentRowId;
        uint64 num_rows;
        uint64 num_fields;

        MYSQL_ROW _firstRow;
        MYSQL_RES* _result;
        Field* _currentRow;
        MYSQL_FIELD* _fields;
};

class Database
{
    public:
        Database();
        ~Database();

        bool Connect(const char* host, const char* user, const char* passwd,
         const char* db = NULL, unsigned int port = 0, const char* unix_socket = NULL, unsigned long client_flag = 0);


        void Close() { connected = false; mysql_close(database); }

        const char* GetError() const { return mysql_error(database); }
        uint32 GetErrno() const { return mysql_errno(database); }

        QueryResult* Query(const char* stmt) const;
        QueryResult* PQuery(const char* stmt, ...) const;

        bool Execute(const char* stmt) const;
        bool PExecute(const char* stmt, ...) const;

        bool IsConnected() const { return connected; }

        uint64 AffectedRows() { return mysql_affected_rows(database); }
        uint64 LastInsertId() { return mysql_insert_id(database); }

        template <class T>
        bool SetOption(mysql_option option, T value)
        {
            return mysqL_options(database, option, &value);
        }

        char* EscapeString(const char* input)
        {
            char* ret = new char[strlen(input) * 2 + 1];
            memset(ret, 0, strlen(input) * 2 + 1);
            mysql_real_escape_string(database, ret, input, strlen(input));
            return ret;
        }

    private:
        MYSQL* database;
        bool connected;


        const char* _host;
        const char* _user;
        const char* _db;
        unsigned int _port;
        const char* _unix_socket;
        unsigned long _client_flag;
};
