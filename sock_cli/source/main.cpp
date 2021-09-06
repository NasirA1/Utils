#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>
#include <iostream>
#include "httplib.h"


#define PORT 80
#define HOST "api.ipify.org"


int socket_client_test()
{
    printf("Socket Client test started\n");
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket\n");
		return -1;
	}
    
	server = gethostbyname(HOST);
    if (server == NULL) {
        printf("ERROR, no such host\n");
        return -1;
    }
    
	bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);
    
	if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) { 
        printf("ERROR connecting\n");
		return -1;
	}
    
    std::string request = "GET / HTTP/1.1\r\nAccept: */*\r\nHost: " HOST "\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\n\r\n";
    printf("Sending request: %s\n", request.c_str());
    n = write(sockfd, request.c_str(), request.length());
    if (n < 0) {
         printf("ERROR writing to socket\n");
		 return -1;
	}
    
    char buffer[1024 * 1024] = {0};
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
         printf("ERROR reading from socket\n");
		 return -1;
	}
    
    printf("Received response: %d bytes\n", n);
	printf("%s\n", buffer);
    
	close(sockfd);
	return 0;
}

void httplib_test()
{
    std::cout << std::endl << "  ----- Started httplib test -----" << std::endl;
    httplib::Client cli(HOST, PORT);
    auto res = cli.Get("/");
    if(res) {
        std::cout << "Response Status: " << res->status << std::endl;
        std::cout << "Response Body: " << res->body << std::endl;
    }
    else {
        std::cout << "Error: " << res.error() << std::endl;
    }
    std::cout << std::endl << "  ----- Ended httplib test -----" << std::endl;
}


int main(int argc, char *argv[])
{
	printf(" *** App Started ****\n");

	socket_client_test();
    httplib_test();

	printf(" *** App Ended ****\n");
    return 0;
}
