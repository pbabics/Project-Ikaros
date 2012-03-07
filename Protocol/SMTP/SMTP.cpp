#include "SMTP.h"


void SMTP::SendGreetings(Socket* sock)
{
    stringstream stream;
    stream << "220 Server-Ikaros " << Greetings << newline;
    sock->Send(stream);
}

void SMTP::SendHelloResponse(Socket* sock, std::string name)
{
    stringstream stream;
    stream << "250 Hello "<< name << newline;
    sock->Send(stream);
}

void SMTP::SendCommandResponse(Socket* sock, int responseCode)
{
    if (responseCodeMessage.find(responseCode) == responseCodeMessage.end())
        return;
    stringstream stream;
    stream << responseCode << ' ' << responseCodeMessage[responseCode] << newline;
    sock->Send(stream);
}
