// name:shep
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
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    char serverAddress[100];
    int  serverPort;
    int    connSock;
    bool startDownload = false;
    struct  sockaddr_in servAddr;
    char sentBuf[BUFFER_SIZE], recvBuf[BUFFER_SIZE];
    int  sentDataLen, recvDataLen;
    string inputServAddress;
    cout << "Enter server address: \n";
    cin >> inputServAddress;
    cin.ignore();

    if (argc >= 3){
        strcpy(serverAddress,argv[1]);
        serverPort = atoi(argv[2]);
    }
    else{
        strcpy(serverAddress,inputServAddress.c_str());
        serverPort = SERVER_PORT;
    }

    connSock = socket(AF_INET, SOCK_STREAM, 0);
    if (connSock < 0){
        cout << "Error when creating socket\n";
        exit(1);
    }

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(serverAddress);
    servAddr.sin_port        = htons(serverPort);
    if (connect(connSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        cout << "Error when connecting server!";
        exit(1);
    }

    cout << "connect to server... \n";
    /*
     * start loop send and recv msg to sever
     * */
    while(1){
        bzero(recvBuf, BUFFER_SIZE);
        cout << "Input a message to the echo server (QUIT for exit): ";
        cin.getline(sentBuf, BUFFER_SIZE);

        
        if (strcmp(sentBuf,"QUIT") == 0 || strcmp(sentBuf,"quit") == 0){
            write(connSock, sentBuf, strlen(sentBuf));
            break;
        }

        sentDataLen = write(connSock, sentBuf, strlen(sentBuf));

        if (sentDataLen < 0){
            cout << "Error when sending data\n";
            break;
        }
        //
        recvDataLen = read(connSock,recvBuf,sizeof(recvBuf));
        if (recvDataLen < 0){
            cout << "Error when receiving data\n";
            break;
        }
        if(!startDownload){
            cout<<"Message received from server: "<<recvBuf<<endl;
        }
    }
    close(connSock);
    return 0;
}