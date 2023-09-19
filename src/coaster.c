// import dependencies
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Define Colour Profiles
// Uncomment as used to keep compiled size down
//
#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
//#define MAGENTA "\033[35m"      /* Magenta */
//#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */
//#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
//#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
//#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
//#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
//#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
//#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
//#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
//#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

// define constants
#define PORT 80
#define DEFAULT_BUFFER_SIZE 1024
#define INTERFACE INADDR_ANY

struct sockaddr_in host_addr;


//
// name: create_socket()
// desc: creates a socket
// args: none
//
int create_socket(){
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, RED "Socket error" RESET);
        return 1;
    }
    fprintf(stdout, GREEN "- socket created successfully\n" RESET);
	return sockfd;
}

//
// name: create_bind_address()
// desc: create address to bind socket to
// args: none
//
struct sockaddr_in create_bind_address(){
    host_addr.sin_family = AF_INET; // ipv4 rather than ipv6
    host_addr.sin_port = htons(PORT); // specified constatnt port
    host_addr.sin_addr.s_addr = htonl(INTERFACE); //any interface

    return host_addr;
}

//
// name: bind_socket()
// desc bind socket to given address
// args: sockfd, host_addrlen
//
int bind_socket(int sockfd, int host_addrlen){
	if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        if (PORT < 1024){
        fprintf(stderr, RED "Bind Error - use SUDO for ports below 1024\n" RESET);
        } else {
        fprintf(stderr, RED "Bind Error\n" RESET);
        }
        return 1;
    }
    fprintf(stdout, GREEN "- socket successfully bound to address\n" RESET);
    return 0;
}

//
// name: listen_for_connections()
// desc: listen for incoming connections
// args: sockfd
//
int listen_for_connections(int sockfd){
	if (listen(sockfd, SOMAXCONN) != 0) {
        fprintf(stderr, RED "Listen Error\n" RESET);
        return 1;
    }
    fprintf(stdout, GREEN "- server listening on %d:%d for connections\n" RESET, host_addr.sin_addr.s_addr, PORT);
}

int accept_incoming_connection(int sockfd, int host_addrlen){
	int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
	if (newsockfd < 0) {
        fprintf(stderr, RED "Accept Error\n" RESET);
        //continue;
	}
    fprintf(stdout, GREEN "- connection accepted\n" RESET);
    return newsockfd;
}

int get_client_address(int newsockfd, struct sockaddr_in client_addr, int client_addrlen){
	int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
	if (sockn < 0) {
		perror("webserver (getsockname)");
		//continue;
	}
}

int read_from_socket(int newsockfd, char* buffer){
	int valread = read(newsockfd, buffer, DEFAULT_BUFFER_SIZE);
    if (valread < 0) {
    	fprintf(stderr, RED "Read Error\n" RESET);
        //continue;
    }
}

int read_request(char* buffer, struct sockaddr_in client_addr){
	char method[DEFAULT_BUFFER_SIZE];
	char uri[DEFAULT_BUFFER_SIZE];
	char version[DEFAULT_BUFFER_SIZE];
	
	sscanf(buffer, "%s %s %s", method, uri, version);
	printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
	
}

int write_to_socket(int newsockfd){
	// default 404 response
	char resp[] = "HTTP/1.0 200 OK\r\n"
	"Server: webserver-c\r\n"
	"Content-type: text/html\r\n\r\n"
	"404 - file not found\r\n";
	
	int valwrite = write(newsockfd, resp, strlen(resp));
    if (valwrite < 0) {
    	fprintf(stderr, RED "Write Error\n" RESET);
        //continue;
    }
}


// main function
int main(){
	// crate buffer to store request
	char buffer[DEFAULT_BUFFER_SIZE];



	// Create client address
	struct sockaddr_in client_addr;
	int client_addrlen = sizeof(client_addr);
	
	fprintf(stdout, GREEN "COASTER - Simple HTTP server in C\n" RESET);
	fprintf(stdout, GREEN "Written by xts-sec\n" RESET);
	fprintf(stdout, GREEN "PRE-RELEASE VERSION\n" RESET);

	// Create a socket
    int sockfd = create_socket();

    // Create the address to bind the socket to
    struct sockaddr_in host_addr = create_bind_address();
    int host_addrlen = sizeof(host_addr);

    // Bind the socket to the address
    bind_socket(sockfd, host_addrlen);

 	// Listen for incoming connections
	listen_for_connections(sockfd);
    
    do {
		// Accept incoming connections
        int newsockfd = accept_incoming_connection(sockfd, host_addrlen);

		// Get client address
		get_client_address(newsockfd, client_addr, client_addrlen);
		
        // Read from the socket
        read_from_socket(newsockfd, buffer);

        // Read the request
		read_request(buffer, client_addr);
                       
		// Write to the socket
        write_to_socket(newsockfd);
        
       	close(newsockfd);
    } while (1);
    
    return 0;
}
