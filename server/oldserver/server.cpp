#include "server.h"
#include "clientsocket.h"

Server::Server()
{
}

Server::~Server()
{

}

void Server::incomingConnection(int socketId)
{

    ClientSocket *socket = new ClientSocket(this);
    socket->setSocketDescriptor(socketId);
}

