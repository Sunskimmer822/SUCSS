#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int WEED_EATER = 48960; //the port number which the socket will attempt to bind to. In the future, all port numbers are randomly generated.

int MAX_REQUEST_SIZE = 2048; //max request size, stored in bytes


int main() {

    //create socket file descriptor using linux syscall
    int sockfd = socket(AF_INET /*IPV4 address space*/, SOCK_STREAM /*TCP socket for reliability*/, 0 /*use default stream type for TCP*/);
    //if the socket creation fails, the sockfd integer will be equal to -1, else 0. 
    //if it fails, print a message to the terminal (or to journalctl, depending on if you are running this as a systemd service or in a terminal) and exit
    if (sockfd == -1) {
        cout << "FATAL: SUCSS cannot create an IPV4 socket. erro: " << errno << "\nExiting." << endl;
        exit(EXIT_FAILURE);
    }

    //defines the socket address and port as existing in the IPV4 address space and looking for any input addresses. binds to port configured in the config file, defaulting to 48960.
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET; //IPV4 address space
    sockaddr.sin_addr.s_addr = INADDR_ANY; //any input address
    sockaddr.sin_port = htons(WEED_EATER); //configurable port - htons() converts the int to network byte order

    //attempts to bind the socket to the predetermined port
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        cout << "FATAL: SUCSS cannot bind to port " << WEED_EATER << ". errno: " << errno << endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        
        //start listening, hold at most 20 connections in the queue
        if (listen(sockfd, 20) < 0) {
            cout << "FATAL: SUCSS cannot listen on the socket. errno: " << errno << endl;
            exit(EXIT_FAILURE);
        }

        //grab a connection from the queue
        auto addrlen = sizeof(sockaddr);
        int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
        if (connection < 0) {
            cout << "FATAL: SUCSS cannot grab the connection. errno: " << errno << endl;
            exit(EXIT_FAILURE);
        }

        //read from the connection
        char buffer[MAX_REQUEST_SIZE];
        auto bytesRead = read(connection, buffer, MAX_REQUEST_SIZE);
        cout << "the message was: " << buffer;

        //send a message to the connection
        string response = "Why did you send me that? you know I hate when people say that.\n";
        send(connection, response.c_str(), response.size(), 0);

        close(connection);
    }
    
    
    close(sockfd);


    return 0;
}