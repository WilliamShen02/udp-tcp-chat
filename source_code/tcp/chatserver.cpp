/**
 * Author: Weilong Shen
 * GTID: 903628514
 * GT Email: wshen61@gatech.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
	if (argc != 6) {
		printf("​Invalid command.\nValid command: ./chatserver_tcp -start -port <port> -passcode <passcode>\n");
		return -1;
	}
	if (strcmp(argv[1], "-start") || strcmp(argv[2], "-port") || strcmp(argv[4], "-passcode")) {
		printf("​Invalid command.\nValid command: ./chatserver_tcp -start -port <port> -passcode <passcode>\n");
		return -1;
	}

	int port = atoi(argv[3]);
	const char *passcode = argv[5];
	if (port < 5000 || port > 7000) {
		printf("​Invalid command.\nPort number must be between 5000 and 7000.\n");
		return -1;
	}

	printf("Server started on port %d. Accepting connections\n", port);
	fd_set file_descriptors;
	int max_descriptor;
	int max_clients = 10;

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 10);

    int client_sockets[max_clients];
    char usernames[100][max_clients];
    for (int i = 0; i < max_clients; i++) {
        client_sockets[i] = 0;
    } 

    while (1) {
    	FD_ZERO(&file_descriptors);  

        FD_SET(server_socket, &file_descriptors);
        FD_SET(0, &file_descriptors);  
        max_descriptor = server_socket;

        for (int i = 0; i < max_clients; i++) {
        	if (client_sockets[i] > 0) {
        		FD_SET(client_sockets[i], &file_descriptors);
        		if (client_sockets[i] > max_descriptor) max_descriptor = client_sockets[i];
        	}
        }

        select(max_descriptor + 1, &file_descriptors, NULL, NULL, NULL);

        if (FD_ISSET(0, &file_descriptors)) {
        	char message[100];
        	bzero(message, sizeof(message));
        	scanf("%s", message);
        	if (!strcmp(message, ":Exit")) {
        		break;
        	}
        	if (!strcmp(message, "listclients")) {
        		for (int i = 0; i < max_clients; i++) {
        			if (client_sockets[i] > 0) {
        				printf("%s ", usernames[i]);
        			}
        		}
        		printf("\n");
        	}
        }

        if (FD_ISSET(server_socket, &file_descriptors)) {
        	for (int i = 0; i < max_clients; i++) {
        		if (client_sockets[i] == 0) {
        			client_sockets[i] = accept(server_socket, NULL, NULL);
        			char code[100];
        			recv(client_sockets[i], &code, sizeof(code), 0);
        			if (strcmp(code, passcode)) {
        				char message[100] = "Incorrect passcode";
        				send(client_sockets[i], message, sizeof(message), 0);
        				close(client_sockets[i]);
        				client_sockets[i] = 0;
        			} else {
        				char message[100] = "Correct passcode\n";
        				send(client_sockets[i], message, sizeof(message), 0);
        				char username[100];
        				recv(client_sockets[i], &username, sizeof(username), 0);
        				strncpy(usernames[i], username, 100);
					    char print[150];
                        bzero(print, sizeof(print));
                        sprintf(print, "%s joined the chatroom", usernames[i]);
                        printf("%s\n", print);
					    for (int j = 0; j < max_clients; j++) {
		        			if (i != j && client_sockets[j] > 0) {
		        				char message[150];
		        				bzero(message, sizeof(message));
		        				sprintf(message, "%s joined the chatroom\n", usernames[i]);
		        				send(client_sockets[j], message, sizeof(message), 0);
		        			}
		        		}
        			}
				    break;
        		}
        	}
        }

        for (int i = 0; i < max_clients; i++) {
        	if (client_sockets[i] > 0 && FD_ISSET(client_sockets[i], &file_descriptors)) {
        		char response[100];
        		bzero(response, sizeof(response));
        		int status = recv(client_sockets[i], &response, sizeof(response), 0);
        		if (!strcmp(response, "***I left the server***")) {
        			printf("%s left the chatroom\n", usernames[i]);
        			for (int j = 0; j < max_clients; j++) {
	        			if (i != j && client_sockets[j] > 0) {
	        				char message[100];
	        				bzero(message, sizeof(message));
	        				sprintf(message, "%s left the chatroom\n", usernames[i]);
	        				send(client_sockets[j], message, sizeof(message), 0);
	        			}
	        		}
	        		break;
        		}
        		if (status == 0) {
        			close(client_sockets[i]);
        			client_sockets[i] = 0;
	        		break;
        		}
        		printf("%s: %s", usernames[i], response);
        		for (int j = 0; j < max_clients; j++) {
        			if (i != j && client_sockets[j] > 0) {
        				char message[150];
        				bzero(message, sizeof(message));
        				sprintf(message, "%s: %s", usernames[i], response);
        				send(client_sockets[j], message, sizeof(message), 0);
        			}
        		}
        		break;
        	}
        }
    }

    for (int i = 0; i < max_clients; i++) {
    	if (client_sockets[i] != 0) {
    		close(client_sockets[i]);
    	}
    }
    close(server_socket);

	return 0;
}

