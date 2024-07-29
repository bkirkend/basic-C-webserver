#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

char* loadhtml(char *filename);

char* loadhtml(char *filename){
  FILE* fptr = fopen(filename, "r");
  if (fptr == NULL) perror("cannot open file\n");
 
  fseek(fptr, 0L, SEEK_END);
  int size = ftell(fptr);
  rewind(fptr);

	const char* headers = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n";

  char* buffer = malloc((size + 1) * sizeof(char));
  if(buffer == NULL) perror("buffer allocation\n");
  fread(buffer, 1, size, fptr);
  buffer[size] = '\0';
  fclose(fptr);

  
  char* resp = malloc((size + strlen(headers) + 1) * sizeof(char));
  strcpy(resp, headers);
  strcat(resp, buffer);

  free(buffer);
  return resp;
}

int main(int argc, char** argv){
	char buffer[BUFFER_SIZE];
  char* resp = loadhtml("main.html");
		

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == 1){
		perror("webserver (socket)");
	}
	printf("socket created successfully\n");

	//Create the address to bind the socket to
	
	struct sockaddr_in host_addr;
	int host_addrlen = sizeof(host_addr);

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&host_addr, 
				host_addrlen) != 0){
		perror("webserver (bind)");
		return 1;
	}

	printf("socket successfully bound to address\n");
		
	if (listen(sockfd, SOMAXCONN) != 0){
		perror("webserver (listen)");
		return 1;
	}

	printf("server listening for connections\n");
	
	while(1){
		int newsockfd = accept(sockfd, (struct sockaddr *) &host_addr,(socklen_t *)&host_addrlen);

		if (newsockfd < 0){
			perror("webserver (accept)");
		}
	
		printf("connection accepted\n");

		int valread = read(newsockfd, buffer, BUFFER_SIZE);
		if (valread < 0){
			perror("webserver (read)");
			continue;
		}

		write(newsockfd, resp, strlen(resp));

		close(newsockfd);

	}
  free(resp);
	return 0;
}

