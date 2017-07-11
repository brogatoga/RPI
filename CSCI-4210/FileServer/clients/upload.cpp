#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


// Main function, entry point of the application
int main(int argc , char *argv[])
{
	// Check command line arguments
	if (argc != 2) {
		std::cout << "Usage: ./upload filename" << std::endl;
		return EXIT_FAILURE;
	}
	
	
	// Make sure that the file exists
	std::string filename = std::string(argv[1]);
	struct stat filestat;
	
	if (access(filename.c_str(), F_OK) == -1) {
		std::cout << "File does not exist" << std::endl;
		return EXIT_FAILURE;
	}
	else stat(filename.c_str(), &filestat);
	
	
	// Connect to the server
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server;
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(8765);
	
	if (sock == -1) {
		std::cout << "Unable to create the socket" << std::endl;
		return EXIT_FAILURE;
	}
	
	if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0) {
		std::cout << "Unable to connect to the server" << std::endl;
		return EXIT_FAILURE;
	}
	
	
	// Send the STORE command along with the file contents
	int size = filestat.st_size + 50;
	char* buffer = new char[size];
	char* contentBuffer = new char[size - 50];
	char sizebuffer[50];
	
	FILE* file = fopen(filename.c_str(), "rb");
	fread(contentBuffer, sizeof(char), size - 50, file);
	fclose(file);
	
	sprintf(sizebuffer, "%d", (int) filestat.st_size);
	strcpy(buffer, "STORE ");
	strcat(buffer, filename.c_str());
	strcat(buffer, " ");
	strcat(buffer, sizebuffer);
	strcat(buffer, "\n");
	strcat(buffer, contentBuffer);
	
	write(sock, buffer, strlen(buffer));
	strcpy(buffer, "");
	read(sock, buffer, size);
	std::cout << buffer << std::endl;
	
	delete[] contentBuffer;
	delete[] buffer;
	
	
	// All done, now close everything and return exit success
	close(sock);
	return EXIT_SUCCESS;
}