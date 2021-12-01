//
// Created by 梵星 on 2021/11/29.
//
//
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

using namespace std;

#define SERVER_PORT 9000
#define BUFFER_SIZE 40960


const string HELLO = "Hello Client";
const string KEYTOINSERT = "Key to Insert";
const string KEY_NOT_FOUND = "Key Not Found";
const string WRONG_SYNTAX = "WRONG SYNTAX";
const string VALUE = "value";

typedef struct{
    int cacheID=4;
    int taskType=3;
    char key[1024];
    char value[1024];
}Packet;
typedef struct
{
    int sock;
    struct sockaddr address;
    int addr_len;
} connection_t;


void * process(void * ptr)
{
    char buffer[BUFFER_SIZE];
    int len;
    Packet packet;
    strcpy(packet.key,KEYTOINSERT.c_str());
    strcpy(packet.value,VALUE.c_str());
    connection_t * conn;

    if (!ptr) pthread_exit(0);
    conn = (connection_t *)ptr;

    bool  ready = false;
    bool wrongSyntax = false;
    while(1) {

        buffer[BUFFER_SIZE] = 0;

        /* read message */
        len = read(conn->sock, buffer, BUFFER_SIZE-1);

        buffer[len] = '\0';

        if(len > 0) {
            /* print message */
            cout << "Received a message from the client: "  << buffer << endl;

            // client close connection
            if (strcmp(buffer,"QUIT") == 0){
                close(conn->sock);
                cout << "Close the connection\n";
                break;
            }

            if(strcmp(buffer, "HELLO") == 0) {
                send(conn->sock, HELLO.c_str(), HELLO.length()+1,0);
            }
            else wrongSyntax = true;
            if(wrongSyntax) {
                memset(buffer,0, BUFFER_SIZE);
                send(conn->sock,(char*)&packet, sizeof (buffer)+1,0);
            }
        }

    }

    /* close socket and clean up */
    close(conn->sock);
    free(conn);
    pthread_exit(0);
}

int main(int argc, char **argv) {
    int sock = -1;
    struct sockaddr_in address;
    int port = 9999;
    connection_t * connection;
    pthread_t thread;

    /* create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0)
    {
        cout << "error: cannot create socket " << argv[0] << endl;
        return -3;
    }

    /* bind socket to port */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if ((::bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in))) < 0)
    {
        cerr << "ERROR: cannot bind socket to port: " << port << endl;
        return -4;
    }

    /* listen on port */
    if (listen(sock, 5) < 0)
    {
        cerr <<  "ERROR: cannot listen on port\n";
        return -5;
    }

    cout << "ready and listening"<<endl;

    while (1)
    {
        /* accept incoming connections */
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock = accept(sock, &connection->address, (socklen_t *) &connection->addr_len);
        cout << "connect to a client" << endl;
        if (connection->sock <= 0)
        {
            free(connection);
        }
        else
        {
            /* start a new thread but do not wait for it */
            pthread_create(&thread, 0, process, (void *)connection);
            pthread_detach(thread);
        }
    }

    return 0;
}
