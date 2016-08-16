#include "server.h"
#include "clientsocket.h"

Server::Server()
{
}

Server::Server(ushort)
{
    pthread_t tid[MAX_THR];

}

Server::~Server()
{

}

void Server::createThreads(pthread_t[])
{
    for (int i = 0; i < MAX_THR; i++){
        order[i] = i;
        if (pthread_create(&tid[i], NULL, accept_request, &order[i]) != 0)
            fprintf(stderr,"Failed to start thread %d\n",i);
    }
}

void Server::incomingConnection(int socketId)
{
    ClientSocket *socket = new ClientSocket(this);
    socket->setSocketDescriptor(socketId);
    std::cout << "Socket id is "<<socketId << "\n";
}
void* Server::accept_request(void *porder){
    int thread_order = *((int*)porder);
    SocketQueue *socket_queue = web_server->getSocketQueue();
    printf("Starting thread %d\n",thread_order);
    for (;;){
        int socket = socket_queue->dequeue();
        if (socket == -1)
            break;
        printf("accepted by thread %d\n",thread_order);
        web_server->serve(socket,thread_order);
        printf("served in thread %d\n",thread_order);
    }
    return NULL;
}

