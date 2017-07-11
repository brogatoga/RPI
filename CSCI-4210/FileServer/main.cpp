#include <iostream>
#include <fstream>
#include <sstream>
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
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define TCP_PORT         8765
#define BUFFER_SIZE      32768
#define STORAGE_DIR      ".storage"
#define BACKLOG_SIZE     5
#define NUM_FRAMES       32
#define FRAME_SIZE       1024
#define FRAMES_PER_FILE  4
#define PAGE_SIZE	     1024
#define DEBUG			 0


// Page structure
struct pageEntry {
	int page;
	std::string file;
	int timestamp;
	bool used;
};
// ----------------


// Global variables
char** frames = NULL;
std::vector<pageEntry> pageTable;
int currentTime = 0;
// ----------------


// Returns the ID of the current thread
std::string getThreadID()
{
	pthread_t tid = pthread_self();
	char buffer[512];
	sprintf(buffer, "%lu", (unsigned long) tid);
	return std::string(buffer);
}


// Generates and returns a timestamp number
int newTime()
{
	int temp = currentTime;
	currentTime++;
	return temp;
}


// Returns the current time
int getTime()
{
	return currentTime;
}


// Returns how many pages a given file has stored in the memory (as frames)
int getPageCountForFile(const std::string& filename)
{
	int count = 0;
	for (int c = 0; c < NUM_FRAMES; c++) {
		if (pageTable[c].used && pageTable[c].file == filename)
			count++;
	}
	return count;
}


// Returns the page index for the page entry with the given filename and page number, or -1 if it doesn't exist
int getPageIndexForEntry(const std::string& filename, int pageNumber)
{
	for (int c = 0; c < NUM_FRAMES; c++) {
		if (pageTable[c].used && pageTable[c].file == filename && pageTable[c].page == pageNumber)
			return c;
	}
	return -1;
}


// Returns the index of the least recently used (LRU) frame for a given file
int getLRUIndexForFile(const std::string& filename = "")
{
	int oldestDifference = 0;
	int oldestIndex = -1;
	
	for (int c = 0; c < NUM_FRAMES; c++) {
		if (pageTable[c].used && (filename.size() == 0 || filename == pageTable[c].file)) {
			int difference = getTime() - pageTable[c].timestamp;
			if (difference > oldestDifference) {
				oldestDifference = difference;
				oldestIndex = c;
			}
		}	
	}
	return oldestIndex;
}


// Returns the index of the least recently used (LRU) frame out of all frames
int getLRUIndex()
{
	return getLRUIndexForFile("");
}


// Stores a page with the given content into memory (as frames)
void storePageIntoMemory(int pageNumber, const std::string& filename, const std::string& content)
{
	// If this file already has 4 frames allocated to it, then find the least recently used (LRU) frame of that file 
	if (getPageCountForFile(filename) >= FRAMES_PER_FILE) {
		int oldest = getLRUIndexForFile(filename);
		if (oldest >= 0) {
			int prevPage = pageTable[oldest].page;
			pageTable[oldest].page = pageNumber;
			pageTable[oldest].timestamp = newTime();
			
			std::cout << "[thread " << getThreadID() << "] Allocated page " << pageNumber << " to frame " << oldest << 
				" (replaced page " << prevPage << ")" << std::endl;
		}
		return;
	}
	
	
	// Iterate through the page table looking for an empty entry
	bool found = false;
	for (int c = 0; c < NUM_FRAMES; c++) {
		if (pageTable[c].used == false) {
			pageTable[c].used = true;
			pageTable[c].file = filename;
			pageTable[c].page = pageNumber;
			pageTable[c].timestamp = newTime();
			strcpy(frames[c], content.c_str());
			std::cout << "[thread " << getThreadID() << "] Allocated page " << pageNumber << " to frame " << c << std::endl;
			// std::cout << filename << std::endl;
			// std::cout << content << std::endl;
			
			found = true;
			break;
		}
	}
	
	
	// If we didn't find an empty entry, then use the LRU algorithm to find and replace the oldest frame out of all frames
	if (!found) {
		int oldest = getLRUIndex();
		if (oldest >= 0) {
			int prevPage = pageTable[oldest].page;
			pageTable[oldest].file = filename;
			pageTable[oldest].page = pageNumber;
			pageTable[oldest].timestamp = newTime();
			
			std::cout << "[thread " << getThreadID() << "] Allocated page " << pageNumber << " to frame " << oldest << 
				" (replaced page " << prevPage << ")" << std::endl;
		}
	}
}
	
	
// Splits a string into multiple pieces based on a delimiter
std::vector<std::string> split(const std::string& input, const char delimiter, int limit = 0)
{
	std::vector<std::string> result;
	std::string piece;
	int length = input.size();

	for (int c = 0; c < length; c++) {
		if (input[c] == delimiter && (limit == 0 || ((int)result.size() < limit - 1))) {
			result.push_back(piece);
			piece.clear();
		}
		else piece += input[c];
	}

	if (piece.size() > 0)
		result.push_back(piece);

	return result;
}


// Sends an error response back to the given client socket
void send_error_response(int socket, const std::string& message)
{
	if (socket <= 0 || message.size() == 0)
		return;

	std::string error_message = "ERROR: " + message + "\n";
	write(socket, error_message.c_str(), error_message.size() + 1);
}


// Sends a message, but only when the DEBUG flag is asserted
void debug(const std::string& message)
{
	if (DEBUG)
		std::cout << "DEBUG: " << message << std::endl;
}


// Handles the STORE command
void command_store(int socket, const std::string& filename, int size, const std::string& content)
{
	// Build the actual destination string
	std::string path = std::string(STORAGE_DIR) + std::string("/") + filename;
	
	
	// Check if any arguments are invalid
	if (filename.size() == 0 || size <= 0 || content.size() == 0) {
		send_error_response(socket, "Cannot STORE file because of invalid arguments");
		return;
	}
	
	
	// Check if the given filename already exists
	if (access(path.c_str(), F_OK) == 0) {
		send_error_response(socket, "FILE EXISTS");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR FILE EXISTS" << std::endl;
		return;
	}
	
	
	// Create a new file with that filename and write the contents to it
	FILE* file = fopen(path.c_str(), "wb");
	if (file == NULL) {
		send_error_response(socket, "CANNOT WRITE TO FILE");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR CANNOT WRITE TO FILE" << std::endl;
		std::cout << "Hi Bo - You can fix this problem by running chmod 777 on the .storage/ folder" << std::endl;
		return;
	}

	fwrite(content.c_str(), sizeof(char), size, file);
	
	if (flock(fileno(file), LOCK_EX) != 0) {
		send_error_response(socket, "Cannot STORE file because unable to acquire a lock");
		fclose(file);
		return;
	}
	flock(fileno(file), LOCK_UN);
	fclose(file);
	chmod(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);
	
	
	// Send response to the user and log server message
	std::cout << "[thread " << getThreadID() << "] Transferred file (" << size << " bytes)" << std::endl;
	std::cout << "[thread " << getThreadID() << "] Sent: ACK" << std::endl;
	write(socket, "ACK\n", 5);
}


// Handles the READ command
void command_read(int socket, const std::string& filename, int offset, int size)
{
	// Build the actual destination string
	std::string path = std::string(STORAGE_DIR) + std::string("/") + filename;
	
	
	// Check if any arguments are invalid
	if (filename.size() == 0) {
		send_error_response(socket, "Cannot READ file because of empty filename");
		return;
	}
	
	
	// Check if the given file exists or not
	if (access(path.c_str(), F_OK) < 0) {
		send_error_response(socket, "NO SUCH FILE");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR NO SUCH FILE" << std::endl;
		return;
	}
	
	
	// Check if the byte range is correct
	struct stat s;
	stat(path.c_str(), &s);
	int fileSize = (int) s.st_size;
	
	if (offset < 0 || offset > fileSize || size > fileSize || offset + size > fileSize) {
		send_error_response(socket, "INVALID BYTE RANGE");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR INVALID BYTE RANGE" << std::endl;
		return;
	}
	
	
	// Read the contents of the given file
	int fd = open(path.c_str(), O_RDONLY);
	int start = offset;
	int end = offset + size - 1;
	std::vector<std::string> pages;
	char buffer[PAGE_SIZE + 1];
	ssize_t nread;
	
	
	// First acquire a shared lock on it
	if (flock(fd, LOCK_SH) != 0) {
		send_error_response(socket, "Cannot READ file because unable to acquire a lock");
		close(fd);
		return;
	}
	
	
	// Store the contents in a vector<string> of pages
	while ((nread = read(fd, buffer, PAGE_SIZE)) > 0) {
		buffer[(int)nread] = '\0';
		pages.push_back(std::string(buffer));
	}
	
	/*
	for (int c = 0; c < pages.size(); c++) {
		std::cout << "Page " << c << ": '" << pages[c] << "'" << std::endl;
	}
	*/
	
	
	// Store the needed pages into memory (frames) and send them back to the client
	for (int c = 0; c < (int)pages.size(); c++) {
		int startPage = c * PAGE_SIZE;
		int endPage = ((c+1) * PAGE_SIZE) - 1;
		
		
		// Do we not need this page?
		if (start > endPage || end < startPage) {
			continue;
		}
		
		
		// Do we need the entire page?
		if (start <= startPage && end >= endPage) {
			
			// Check and see if the page is already stored in a frame
			int index = getPageIndexForEntry(filename, c);
			if (index < 0)
				storePageIntoMemory(c, filename, pages[c]);
			
			
			// Send response
			char response[PAGE_SIZE * 2];
			sprintf(response, "ACK %d\n%s", PAGE_SIZE, (index >= 0) ? frames[index] : pages[c].c_str());
			write(socket, response, strlen(response));
			
			std::cout << "[thread " << getThreadID() << "] Sent: ACK " << PAGE_SIZE << std::endl;
			std::cout << "[thread " << getThreadID() << "] Transferred " << PAGE_SIZE << " bytes from offset " << startPage << std::endl;
		}
		
		
		// Or do we need to read from N to end of page?
		else if (start >= startPage && end > endPage) {
			
			// Check and see if the page is already stored in a frame
			int index = getPageIndexForEntry(filename, c);
			if (index < 0)
				storePageIntoMemory(c, filename, pages[c]);
			
			// Send response
			char buffer2[PAGE_SIZE];
			int counter = 0;
			int d;
			
			if (index < 0) {
				for (d = start - startPage; d <= endPage - startPage && d < (int)pages[c].size(); d++) {
					buffer2[counter++] = pages[c][d];
				}
			}
			else {
				for (d = start - startPage; d <= endPage - startPage && d < (int)strlen(frames[index]); d++) {
					buffer2[counter++] = frames[index][d];
				}
			}
			
			buffer2[counter] = '\0';
			int length2 = strlen(buffer2);
			
			char response[PAGE_SIZE * 2];
			sprintf(response, "ACK %d\n%s", length2, buffer2);
			write(socket, response, strlen(response));
			std::cout << "[thread " << getThreadID() << "] Sent: ACK " << length2 << std::endl;
			std::cout << "[thread " << getThreadID() << "] Transferred " << length2 << " bytes from offset " << start << std::endl;
		}
		
		
		// Or do we need to read from start of page to N?
		else if (start < startPage && end <= endPage) {
			
			// Check and see if the page is already stored in a frame
			int index = getPageIndexForEntry(filename, c);
			if (index < 0)
				storePageIntoMemory(c, filename, pages[c]);
			
			
			// Send response
			char buffer2[PAGE_SIZE];
			int counter = 0;
			int d;
			
			if (index < 0) {
				for (d = 0; d <= end - startPage && d < (int)pages[c].size(); d++) {
					buffer2[counter++] = pages[c][d];
				}
			}
			else {
				for (d = 0; d <= end - startPage && d < (int)strlen(frames[index]); d++) {
					buffer2[counter++] = frames[index][d];
				}
			}
			buffer2[counter] = '\0';
			int length2 = strlen(buffer2);
			
			char response[PAGE_SIZE * 2];
			sprintf(response, "ACK %d\n%s", length2, buffer2);
			write(socket, response, strlen(response));
			
			std::cout << "[thread " << getThreadID() << "] Sent: ACK " << length2 << std::endl;
			std::cout << "[thread " << getThreadID() << "] Transferred " << length2 << " bytes from offset " << startPage << std::endl;
		}
		
		
		// Or do we just need one bounded part of a single page? (could be a single entire page too)
		else if (start >= startPage && end <= endPage) {
			
			// Check and see if the page is already stored in a frame
			int index = getPageIndexForEntry(filename, c);
			if (index < 0)
				storePageIntoMemory(c, filename, pages[c]);
			
			
			// Send response
			char buffer2[PAGE_SIZE];
			int counter = 0;
			int d;
			
			if (index < 0) {
				for (d = start - startPage; d <= end - startPage && d < (int)pages[c].size(); d++) {
					buffer2[counter++] = pages[c][d];
				}
			}
			else {
				for (d = start - startPage; d <= end - startPage && d < (int)strlen(frames[index]); d++) {
					buffer2[counter++] = frames[index][d];
				}
			}
			buffer2[counter] = '\0';
			int length2 = strlen(buffer2);
			
			char response[PAGE_SIZE * 2];
			sprintf(response, "ACK %d\n%s", length2, buffer2);
			write(socket, response, strlen(response));
			
			std::cout << "[thread " << getThreadID() << "] Sent: ACK " << length2 << std::endl;
			std::cout << "[thread " << getThreadID() << "] Transferred " << length2 << " bytes from offset " << startPage << std::endl;
		}
	}
	
	
	// Unlock and close the file
	flock(fd, LOCK_UN);
	close(fd);
}


// Handles the DELETE command
void command_delete(int socket, const std::string& filename)
{
	std::string path = std::string(STORAGE_DIR) + std::string("/") + filename;
	
		
	// Check if any arguments are invalid
	if (filename.size() == 0) {
		send_error_response(socket, "Cannot DELETE file beause of empty filename");
		return;
	}
	
	
	// Check if the given file exists or not
	if (access(path.c_str(), F_OK) < 0) {
		send_error_response(socket, "NO SUCH FILE");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR NO SUCH FILE" << std::endl;
		return;
	}
	
	
	// Acquire an exclusive lock on the file
	int fd = open(path.c_str(), O_RDONLY);
	
	if (flock(fd, LOCK_EX) != 0) {
		send_error_response(socket, "Unable to DELETE file because the server couldn't acquire a lock on it");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR COULD NOT ACQUIRE LOCK" << std::endl;
		return;
	}
	
	if (remove(path.c_str()) == 0) {
		
		// Remove all frames associated with that file
		for (int c = 0; c < NUM_FRAMES; c++) {
			if (pageTable[c].file == filename) {
				pageTable[c].used = false;
				std::cout << "[thread " << getThreadID() << "] Deallocated frame " << c << std::endl;
			}
		}
		
		write(socket, "ACK\n", 5);
		std::cout << "[thread " << getThreadID() << "] Deleted " << filename << " file" << std::endl;
		std::cout << "[thread " << getThreadID() << "] Sent: ACK" << std::endl;
	}
	else {
		send_error_response(socket, "Unable to DELETE file because of unknown error");
		std::cout << "[thread " << getThreadID() << "] Sent: ERROR UKNOWN ERROR OCCURRED WHEN DELETING FILE" << std::endl;
		return;
	}
	
	
	flock(fd, LOCK_UN); 
	close(fd);
}


// Handles the DIR command
void command_dir(int socket)
{
	DIR* dir = opendir(STORAGE_DIR);
	struct dirent* entry;
	std::vector<std::string> filenames;
	
	// If the directory doesn't exist, then we can reply that there are no files in the storage directory
	if (dir == NULL) {
		write(socket, "0\n", 3);
		return;
	}
	
	
	// Go through the directory and add all of the files into a vector
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			filenames.push_back(std::string(entry->d_name));
	}
	closedir(dir);
	
	
	// Send output to the user
	char s[20];
	sprintf(s, "%d\n", (int)filenames.size());
	write(socket, s, strlen(s) + 1);
	
	for (size_t c = 0; c < filenames.size(); c++) {
		write(socket, filenames[c].c_str(), filenames[c].size() + 1);
		write(socket, "\n", 2);
	}
}


// Given a message string, processes it by calling the corresponding command handler function
void process_message(int socket, const std::string& message)
{
	// Parse the message by breaking it up into pieces based on delimiters
	std::vector<std::string> lines = split(std::string(message), '\n', 2);
	std::vector<std::string> pieces = split(lines[0], ' ');
	int num_lines = lines.size();
	int num_pieces = pieces.size();
	std::string command = (num_pieces > 0) ? pieces[0] : "";


	// Print server message
	std::cout << "[thread " << getThreadID() << "] Rcvd: " << lines[0] << std::endl;

	// Do something based on the command type and then send a response back
	if (command == "STORE") {
		if (num_pieces != 3 && num_lines != 2) send_error_response(socket, std::string("Wrong number of arguments for the STORE command"));
		else command_store(socket, pieces[1], atoi(pieces[2].c_str()), lines[1]);
	}
	
	else if (command == "READ") {
		if (num_pieces != 4) send_error_response(socket, std::string("Wrong number of arguments for the READ command"));
		else command_read(socket, pieces[1], atoi(pieces[2].c_str()), atoi(pieces[3].c_str()));
	}
	else if (command == "DELETE") {
		if (num_pieces != 2) send_error_response(socket, std::string("Wrong number of arguments for the DELETE command"));
		else command_delete(socket, pieces[1]);
	}
	else if (command == "DIR") {
		if (num_pieces != 1) send_error_response(socket, std::string("Wrong number of arguments for the DIR command"));
		else command_dir(socket);
	}
	else send_error_response(socket, std::string("You have entered in an invalid command"));
}


// Thread that handles every client connection request
void* handle_response(void* socket_data)
{
	// Variable declarations
	int socket = *((int*) socket_data);
	std::string message;
	char buffer[BUFFER_SIZE];
	ssize_t bytes_read;
	bool store = false;
	int remaining = 0;


	// Repeatedly receive messages until we think we have a full message, then try to process it
	while ((bytes_read = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
		buffer[bytes_read] = '\0';
		message += std::string(buffer);
		
		
		// If we receive the first line of a STORE command, then set the flag to true
		if (!store && message.substr(0, 5) == "STORE") {
			std::vector<std::string> pieces = split(message, ' ');
			int size = atoi(pieces[1].c_str());
			
			// Did it send everything all at once?
			if ((int)message.length() >= size) {
				process_message(socket, message);
				store = false;
				remaining = 0;
				message = "";
			}
			
			// Or are the packets fragmented?
			else {
				store = true;
				std::vector<std::string> lines = split(message, '\n', 2);
				remaining = size - lines[1].size();
			}
		}
		
		
		// If we have already received the first line of the STORE command, then 
		// get the rest of the file contents in multiple recv calls.
		else if (store) {
			remaining -= bytes_read;
			if (remaining <= 2) {
				process_message(socket, message);
				remaining = 0;
				store = false;
				message = "";
			}
		}
		
		
		// If it's not a STORE command, then we can process the response after seeing a newline at the end
		else if (!store && *message.rbegin() == '\n') {
			process_message(socket, message);
			message = "";
			store = false;
		}
	}
		
		
	// Close the socket and exit the thread
	std::cout << "[thread " << getThreadID() << "] Client closed its socket....terminating" << std::endl;
	close(socket);
	return NULL;
}


// Main function, entry point of the application
int main(int argc, char *argv[])
{
	// Process addtitional command line arguments
	if (argc > 1) {
	}


	// Create the storage directory if it doesn't already exist
	struct stat s;
	if (stat(STORAGE_DIR, &s) == -1) {
		mkdir(STORAGE_DIR, 0777);
	}


	// Allocate memory for the frames
	frames = new char* [NUM_FRAMES];
	for (int c = 0; c < NUM_FRAMES; c++) {
		frames[c] = new char[FRAME_SIZE];
	}
	
	
	// Initialize the page table
	for (int c = 0; c < NUM_FRAMES; c++) {
		struct pageEntry pe;
		pe.used = false;
		pageTable.push_back(pe);
	}


	// Create the socket and start listening
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int server_length;
	int client_length;
	char client_hostname[256];

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(TCP_PORT);
	server_length = sizeof(server_address);

	if (sock < 0) {
		perror("Failed to create the socket");
		exit(EXIT_FAILURE);
	}

	if (bind(sock, (struct sockaddr*) &server_address, server_length) < 0) {
		perror("Socket binding failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sock, BACKLOG_SIZE) < 0) {
		perror("Failed to start listening");
		exit(EXIT_FAILURE);
	}
	
	
	// Output starting messages
	std::cout << "Welcome to Ryan's File Storage Server!" << std::endl;
	std::cout << "These are the commands that you can run: " << std::endl;
	std::cout << "STORE <filename> <bytes>\\n<file-contents>  --  Uploads a file to the server" << std::endl;
	std::cout << "READ <filename> <byte-offset> <length>  --  Reads the contents of a file" << std::endl;
	std::cout << "DELETE <filename>  --  Removes the specified file from the server" << std::endl;
	std::cout << "DIR  --  Return a list of files currently stored on the server" << std::endl << std::endl;
	std::cout << "Listening on port " << TCP_PORT << std::endl;


	// Start responding to connection requests, and don't stop until the program is terminated
	while (true) {
		int client = accept(sock, (struct sockaddr*) &client_address, (socklen_t*) &client_length);
		pthread_t thread_id;

		inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_hostname, INET_ADDRSTRLEN);
		std::cout << "Received incoming connection from " << client_hostname << std::endl;

		if (pthread_create(&thread_id, NULL, handle_response, (void*) &client) < 0) {
			perror("Failed to create new thread");
			exit(EXIT_FAILURE);
		}
	}


	// Free all allocated memory
	for (int c = 0; c < NUM_FRAMES; c++) {
		delete[] frames[c];
	}
	delete[] frames;


	// Exit application with success code
	close(sock);
	return EXIT_SUCCESS;
}