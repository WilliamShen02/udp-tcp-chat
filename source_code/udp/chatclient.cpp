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
#include <time.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
    if (argc != 10) {
        printf("​Invalid command.\nValid command: ./chatclient_udp -join -host <hostname> -port <port> -username <username> -passcode <passcode>\n");
        return -1;
    }
    if (strcmp(argv[1], "-join") || strcmp(argv[2], "-host") || strcmp(argv[4], "-port") || strcmp(argv[6], "-username") || strcmp(argv[8], "-passcode")) {
        printf("​Invalid command.\nValid command: ./chatclient_udp -join -host <hostname> -port <port> -username <username> -passcode <passcode>\n");
        return -1;
    }
    if (strcmp(argv[3], "127.0.0.1")) {
        printf("Invalid Host. Must be 127.0.0.1\n");
        return -1;
    }

    const char *host = argv[3]; 
    int port = atoi(argv[5]);
    const char *username = argv[7];
    const char *passcode = argv[9];
    if (port < 5000 || port > 7000) {
        printf("​Invalid command.\nPort number must be between 5000 and 7000.\n");
        return -1;
    }

    fd_set file_descriptors;
    int max_descriptor;
	int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(host);

    int len = sizeof(server_address);

    char isPasscode[100] = "***PASSCODE***";

    sendto(client_socket, isPasscode, sizeof(isPasscode), 0, (const struct sockaddr *) &server_address, len);

    sendto(client_socket, passcode, sizeof(passcode), 0, (const struct sockaddr *) &server_address, len);

    char response[100];
    recvfrom(client_socket, response, sizeof(response), 0, (struct sockaddr *) &server_address, (socklen_t *) &len);
    if (!strcmp(response, "Incorrect passcode")) {
        printf("%s\n", response);
        return -1;
    }

    printf("Connected to %s on port %d\n", host, port);

    char name[100];
    bzero(name, sizeof(name));
    memcpy(name, username, sizeof(name));
    sendto(client_socket, name, sizeof(name), 0, (const struct sockaddr *) &server_address, len);

    while (1) {
        FD_ZERO(&file_descriptors);  

        FD_SET(0, &file_descriptors);  
        FD_SET(client_socket, &file_descriptors);

        max_descriptor = client_socket;

        select(max_descriptor + 1, &file_descriptors, NULL, NULL, NULL);

        if (FD_ISSET(client_socket, &file_descriptors)) {
            char response[100];
            bzero(response, sizeof(response));
            int status = recvfrom(client_socket, response, sizeof(response), 0, (struct sockaddr *) &server_address, (socklen_t *) &len);
            printf("%s", response);
            if (status == 0) break;
        }

        if (FD_ISSET(0, &file_descriptors)) {
            char message[100];
            bzero(message, sizeof(message));
            int index = 0;
            while (1) {
                scanf("%c", message + index);
                if (message[index] == '\n') break;
                if (index > 99) break;
                index++;
            }

            char s[] = " ";
            char *token;
            char result[100];
            bzero(result, sizeof(result));

            token = strtok(message, s);
            while(token != NULL) {
                if (!strcmp(token, ":)")) strcat(result, "[feeling happy]");
                else if (!strcmp(token, ":)\n")) strcat(result, "[feeling happy]\n");
                else if (!strcmp(token, ":(")) strcat(result, "[feeling sad]");
                else if (!strcmp(token, ":(\n")) strcat(result, "[feeling sad]\n");
                else if (!strcmp(token, ":mytime") || !strcmp(token, ":mytime\n")) {
                    time_t t;
                    time(&t);
                    struct tm *info;
                    info = localtime(&t);
                    char *time = asctime(info);
                    strncat(result, time, strlen(time) - 1);
                    if (!strcmp(token, ":mytime\n")) strcat(result, "\n");
                }
                else if (!strcmp(token, ":+1hr") || !strcmp(token, ":+1hr\n")) {
                    time_t t;
                    time(&t);
                    t += 3600;
                    struct tm *info;
                    info = localtime(&t);
                    char *time = asctime(info);
                    strncat(result, time, strlen(time) - 1);
                    if (!strcmp(token, ":+1hr\n")) strcat(result, "\n");
                }
                else strcat(result, token);
                token = strtok(NULL, s);
                if (token != NULL) strcat(result, " ");
            }
            if (!strcmp(result, ":Exit\n")) {
                break;
            }
            sendto(client_socket, result, sizeof(result), 0, (const struct sockaddr *) &server_address, len);

        }
    }
    
    char message[100] = "***I left the server***";
    sendto(client_socket, message, sizeof(message), 0, (const struct sockaddr *) &server_address, len);
    close(client_socket);

	return 0;
}