#include "HTTP.h"

void* SendOverNewThread(void* args)
{
    SeparateThreadedSendingData* data = (SeparateThreadedSendingData*) args;
    if (!data->sock)
        return NULL;
    if (data->dat)
    {
        data->sock->Send(*data->dat, MSG_NOSIGNAL);
        delete data->dat;
    }
    else
    if (data->stream)
    {
        data->sock->Send(*data->stream, MSG_NOSIGNAL);
        delete data->stream;
    }
    else
    if (data->data)
    {
        data->sock->Send(data->data, data->length, MSG_NOSIGNAL);
    }
    delete data;
    return NULL;
}


void HTTPResponse::SendErrorPage(Socket* socket, uint32 ErrorId)
{
    if (errorStrings.find(ErrorId) == errorStrings.end())
        return;
    if (errorPages.find(ErrorId) == errorStrings.end())
        return;

    stringstream* stream = new stringstream;

    string content = string(errorPages[ErrorId]);

    *stream << "HTTP/1.1 ";
    *stream << ErrorId << " " << errorStrings[ErrorId] << newline;
    *stream << "Server: Ikaros" << newline;
    *stream << "Host: " << stringConfigs[CONFIG_STRING_HOSTNAME] << newline;
    *stream << "Connection: close" << newline;
    *stream << "Content-Length: " << content.length() << newline;
    //stream << "Content-Type: text/html" << newline ;
    *stream << newline;
    *stream << content;

    SeparateThreadedSendingData* threadData = new SeparateThreadedSendingData(socket, stream);
    if (content.length() > size_t(intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE]))
        Thread::CreateThread(SendOverNewThread, (void*) threadData);
    else
    {
        socket->Send(*stream);
        delete stream;
    }
}


void HTTPResponse::SendDirectPage(Socket* socket, string file)
{
    BinnaryData* stream = new BinnaryData();
    Files::BinFile content;
    content.readFile(stringConfigs[CONFIG_STRING_ROOT_DIR] + file);

    *stream << "HTTP/1.1 200 OK" << newline;
    *stream << "Server: Ikaros" << newline;
    *stream << "Host: " << stringConfigs[CONFIG_STRING_HOSTNAME] << newline;
    *stream << "Connection: close" << newline;
    *stream << "Content-Length: " << toString(content.getLength()).c_str() << newline;
    //stream << "Content-Type: text/html" << newline ;
    *stream << newline ;
    stream->append(content.toByteArray(), content.getLength());

    SeparateThreadedSendingData* threadData = new SeparateThreadedSendingData(socket, stream);
    if (stream->size() > size_t(intConfigs[CONFIG_INT_MAX_MAIN_THREAD_FILE_SIZE]))
        Thread::CreateThread(SendOverNewThread, (void*) threadData);
    else
    {
        socket->Send(*stream);
        delete stream;
        delete threadData;
    }
    content.clear();
}

void HTTPResponse::SendHeadPage(Socket* socket, string file)
{
    stringstream stream;
    Files::AsciiFile content;
    content.sreadFile(stringConfigs[CONFIG_STRING_ROOT_DIR] + file);

    stream << "HTTP/1.1 200 OK" << newline;
    stream << "Server: Ikaros" << newline;
    stream << "Host: " << stringConfigs[CONFIG_STRING_HOSTNAME] << newline;
    stream << "Connection: close" << newline;
    stream << "Content-Length: " << content.getLength() << newline;
    //stream << "Content-Type: text/html" << newline << newline;

    socket->Send(stream);
}

void HTTPResponse::KillSpartans(Socket* socket)
{
    stringstream stream;
    string str = "I'm Going TO KILL YARR SPARTANS!!!!";

    stream << "HTTP/1.1 200 OK" << newline;
    stream << "Server: Ikaros" << newline;
    stream << "Host: " << stringConfigs[CONFIG_STRING_HOSTNAME] << newline;
    stream << "Connection: close" << newline;
    stream << "Content-Length: " << str.length() << newline;
    stream << "Content-Type: text/html" << newline ;
    stream << newline ;
    stream << str;

    socket->Send(stream);
}
