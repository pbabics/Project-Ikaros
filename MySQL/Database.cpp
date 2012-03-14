#include "Database.h"

const char* DuplicateString(const char* _a)
{
    if (!_a)
        return NULL;
    char* b = new char[strlen(_a) + 1];
    memset(b, 0, strlen(_a) + 1);
    strcpy(b, _a);
    return const_cast<const char*>(b);
}

Database::Database(): 
connected(false), _host(NULL), _user(NULL), _db(NULL), _port(0), _unix_socket(NULL), _client_flag(0)
{
    database = mysql_init(NULL);
}

Database::~Database()
{
    mysql_close(database);
}

bool Database::Connect(const char* host, const char* user, const char* passwd, const char* db, unsigned int port, const char* unix_socket, unsigned long client_flag)
{
    if (_host)
        delete _host;
    _host = DuplicateString(host);
    if (_user)
        delete _user;
    _user = DuplicateString(user);
    if (_db)
        delete _db;
    _db = DuplicateString(db);

    _port = port;
    if (_unix_socket)
        delete _unix_socket;
    _unix_socket = DuplicateString(unix_socket);

    _client_flag = client_flag;


    return connected = (mysql_real_connect(database, _host, _user, passwd, _db, port, unix_socket, client_flag) != NULL );
}


QueryResult* Database::Query(const char* stmt) const
{
    int res = mysql_query(database, stmt);
    if (res == 0)
    {
        MYSQL_RES* _result = mysql_store_result(database);
        return new QueryResult(_result, mysql_fetch_row(_result), mysql_fetch_fields(_result));
    }
    return NULL;
}

QueryResult* Database::PQuery(const char* stmt, ...) const
{
    va_list args;
    char sql [MAX_SQL_BUFFER];
    va_start(args, stmt);
    vsnprintf(sql, MAX_SQL_BUFFER, stmt, args);
    va_end(args);

    int res = mysql_query(database, sql);
    if (res == 0)
    {
        MYSQL_RES* _result = mysql_store_result(database);
        MYSQL_ROW _row = mysql_fetch_row(_result);
        MYSQL_FIELD* _fields = mysql_fetch_fields(_result);
        if (!_row)
            return NULL;
        return new QueryResult(_result, _row, _fields);
    }
    return NULL;
}

bool Database::Execute(const char* stmt) const
{
    return mysql_query(database, stmt) == 0;
}

bool Database::PExecute(const char* stmt, ...) const
{
    va_list args;
    char sql[MAX_SQL_BUFFER];
    va_start(args, stmt);
    vsnprintf(sql, MAX_SQL_BUFFER, stmt, args);
    va_end(args);
    return mysql_query(database, sql) == 0;
}



QueryResult::QueryResult(MYSQL_RES* result, MYSQL_ROW row, MYSQL_FIELD* fields):
_result(result), _fields(fields)
{
    num_rows = mysql_num_rows(_result); 
    num_fields = mysql_num_fields(_result); 
    _currentRow = new Field[num_fields];
    _currentRowId = 0;
    FillFields(row, fields);
}

QueryResult::~QueryResult() 
{
    if (_result)
    {
        mysql_free_result(_result);
        _result = NULL;
    }
    if (_currentRow)
    {
        delete [] _currentRow;
        _currentRow = NULL;
    }
}

void QueryResult::FillFields(MYSQL_ROW row, MYSQL_FIELD* fields)
{
    for (uint32 i = 0; i < num_fields; ++i)
        _currentRow[i].SetStructureData(row[i], fields[i]);
}

bool QueryResult::NextRow()
{
    if (!_result)
        return false;

    MYSQL_ROW row = mysql_fetch_row(_result);
    if (!row)
        return false;

    _currentRowId++;
    FillFields(row, _fields);
    return true;
}

