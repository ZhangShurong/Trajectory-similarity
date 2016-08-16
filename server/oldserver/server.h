#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <pthread.h>
#define MAX_THR (10) // Size of thread pool
class Server :public QTcpServer
{
public:
    Server();
    Server(ushort);
    ~Server();
private:
    int order[MAX_THR];
    static void* accept_request(void *pid);
    void createThreads(pthread_t []);
    void incomingConnection(int socketId);
};

#endif // SERVER_H
