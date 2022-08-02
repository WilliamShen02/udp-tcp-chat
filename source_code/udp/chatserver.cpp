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
		printf("​Invalid command.\nValid command: ./chatserver_udp -start -port <port> -passcode <passcode>\n");
		return -1;
	}
	if (strcmp(argv[1], "-start") || strcmp(argv[2], "-port") || strcmp(argv[4], "-passcode")) {
		printf("​Invalid command.\nValid command: ./chatserver_udp -start -port <port> -passcode <passcode>\n");
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

	int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    struct sockaddr_in clients[max_clients];
    int clients_len[max_clients];
    for (int i = 0; i < max_clients; i++) {
        memset(&clients[i], 0, sizeof(clients[i]));
        clients_len[i] = sizeof(clients[i]);
    }
    char usernames[100][max_clients];

    while (1) {
    	FD_ZERO(&file_descriptors);  

        FD_SET(server_socket, &file_descriptors);
        FD_SET(0, &file_descriptors);  
        max_descriptor = server_socket;

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
        			if (ntohs(clients[i].sin_port) > 0) {
        				printf("%s ", usernames[i]);
        			}
        		}
        		printf("\n");
        	}
        }

        if (FD_ISSET(server_socket, &file_descriptors)) {
            int found = 0;
            struct sockaddr_in client;
            int len = sizeof(client);
            char message[100];
            recvfrom(server_socket, message, sizeof(message), 0, (struct sockaddr *) &client, (socklen_t *) &len);
            for (int i = 0; i < max_clients; i++) {
                if (ntohs(clients[i].sin_port) == ntohs(client.sin_port)) {
                    found = 1;
                    if (!strcmp(message, "***I left the server***")) {
                        printf("%s left the chatroom\n", usernames[i]);
                        for (int j = 0; j < max_clients; j++) {
                            if (i != j && ntohs(clients[j].sin_port) > 0) {
                                char mes[100];
                                bzero(mes, sizeof(mes));
                                sprintf(mes, "%s left the chatroom\n", usernames[i]);
                                sendto(server_socket, mes, sizeof(mes), 0, (const struct sockaddr *) &clients[j], clients_len[j]);
                                memset(&clients[i], 0, sizeof(clients[i]));
                                bzero(usernames[i], sizeof(usernames[i]));
                            }
                        }
                        break;
                    }
                    printf("%s: %s", usernames[i], message);
                    for (int j = 0; j < max_clients; j++) {
                        if (i != j && ntohs(clients[j].sin_port) > 0) {
                            char mes[150];
                            bzero(mes, sizeof(mes));
                            sprintf(mes, "%s: %s", usernames[i], message);
                            sendto(server_socket, mes, sizeof(mes), 0, (const struct sockaddr *) &clients[j], clients_len[j]);
                        }
                    }
                    break;
                }
            }
            if (!found) {
            	for (int i = 0; i < max_clients; i++) {
            		if (ntohs(clients[i].sin_port) == 0) {
            			char code[100];
            			recvfrom(server_socket, code, sizeof(code), 0, (struct sockaddr *) &clients[i], (socklen_t *) &clients_len[i]);
            			if (strcmp(code, passcode)) {
            				char message[100] = "Incorrect passcode";
            				sendto(server_socket, message, sizeof(message), 0, (const struct sockaddr *) &clients[i], clients_len[i]);
            				memset(&clients[i], 0, sizeof(clients[i]));
            			} else {
            				char message[100] = "Correct passcode\n";
                            sendto(server_socket, message, sizeof(message), 0, (const struct sockaddr *) &clients[i], clients_len[i]);
            				char username[100];
                            recvfrom(server_socket, username, sizeof(username), 0, (struct sockaddr *) &clients[i], (socklen_t *) &clients_len[i]);
                            strncpy(usernames[i], username, 100);
    					    char print[150];
                            bzero(print, sizeof(print));
                            sprintf(print, "%s joined the chatroom", usernames[i]);
                            printf("%s\n", print);
    					    for (int j = 0; j < max_clients; j++) {
    		        			if (i != j && ntohs(clients[j].sin_port) > 0) {
    		        				char message[150];
    		        				bzero(message, sizeof(message));
    		        				sprintf(message, "%s joined the chatroom\n", usernames[i]);
    		        				sendto(server_socket, message, sizeof(message), 0, (const struct sockaddr *) &clients[j], clients_len[j]);
    		        			}
    		        		}
            			}
    				    break;
            		}
            	}
            }
        }
    }

    close(server_socket);

	return 0;
}

