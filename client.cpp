// name:shepherd


#include <cstdio>
#include <stdlib.h>
#include <string>
#include <sys/types.h>       // basic system data types
#include <sys/socket.h>      // basic socket definitions
#include <netinet/in.h>      // sockaddr_in{} and other Internet defns
#include <arpa/inet.h>       // inet(3) functions
#include <sys/un.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define SERVER_PORT 9999
#define BUFFER_SIZE 4096

#define INSERT 0
#define GETVALUE 1
const string VALUETEST = "valuetest";
typedef struct {
    int cacheID;
    int taskType;
    char key[1024];
    char value[1024];
} recvInfo;

int main(int argc, char **argv) {
    recvInfo revbuf;
    char serverAddress[100];
    int serverPort;
    int connSock;
    bool startDownload = false;
    struct sockaddr_in servAddr;
    char sentcharBuf[BUFFER_SIZE], recvBuf[BUFFER_SIZE];
    int sentDataLen, recvDataLen;
    recvInfo psend;
    string inputServAddress;
    cout << "Enter server address: \n";
    cin >> inputServAddress;
    cin.ignore();

    if (argc >= 3) {
        strcpy(serverAddress, argv[1]);
        serverPort = atoi(argv[2]);
    } else {
        strcpy(serverAddress, inputServAddress.c_str());
        serverPort = SERVER_PORT;
    }

    connSock = socket(AF_INET, SOCK_STREAM, 0);
    if (connSock < 0) {
        cout << "Error when creating socket\n";
        exit(1);
    }

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(serverAddress);
    servAddr.sin_port = htons(serverPort);
    if (connect(connSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        cout << "Error when connecting server!";
        exit(1);
    }

    cout << "connect to server... \n";
    /*
     * start loop send and recv msg to sever
     * */
    while (1) {
        bzero(recvBuf, BUFFER_SIZE);

        cout << "Input a message to the echo server (QUIT for exit): ";
        //cin.getline(sentcharBuf, BUFFER_SIZE);
        if (strcmp(sentcharBuf, "QUIT") == 0 || strcmp(sentcharBuf, "quit") == 0) {
            send(connSock, sentcharBuf, strlen(sentcharBuf)+1,0);
            break;
        }
        psend.cacheID = 10;
        psend.taskType = 1;
        strcpy(psend.key, VALUETEST.c_str());
        cout << "Message Struct" << endl
             << "cache ID: " << psend.cacheID << endl
             << "tasktype: " << psend.taskType << endl
             << "key: " << psend.key << endl;
        strcpy(psend.value, VALUETEST.c_str());
        sentDataLen = send(connSock, (char*)&psend, sizeof(psend)+1,0);

        if (sentDataLen < 0) {
            cout << "Error when sending data\n";
            break;
        }
        //
        recvDataLen = recv(connSock, recvBuf, sizeof(recvBuf)+1,0);
        if (recvDataLen < 0) {
            cout << "Error when receiving data\n";
            break;
        } else {
            memset(&revbuf, 0, sizeof(revbuf));
            memcpy(&revbuf, recvBuf, sizeof(revbuf));

            cout << "Message received from server: " << recvBuf << endl;
            cout << "Message Struct" << endl
                 << "cache ID: " << revbuf.cacheID << endl
                 << "tasktype: " << revbuf.taskType << endl
                 << "key: " << revbuf.key << endl;
        }
    }
    close(connSock);
    return 0;
}
