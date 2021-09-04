#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string>

#define PORT 8080
#define HOST "127.0.0.1"



int client_test()
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
		return -1;
	}
    
	server = gethostbyname(HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }
    
	bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);
    
	if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) { 
        perror("ERROR connecting");
		return -1;
	}
    
	std::string request = "GET / HTTP/1.1\n\r\nHost: " HOST "\n\r\n";
    n = write(sockfd, request.c_str(), request.length());
    if (n < 0) {
         perror("ERROR writing to socket");
		 return -1;
	}
    
    char buffer[1024 * 1024] = {0};
    n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
         perror("ERROR reading from socket");
		 return -1;
	}
    
	printf("%s\n", buffer);
    
	close(sockfd);
	return 0;
}


int main(int argc, char *argv[])
{
	return client_test();
}
