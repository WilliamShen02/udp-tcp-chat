# TCP/UDP Chat Application
## Weilong Shen @ Georgia Tech

Welcome to the UDP/TCP Chat Application project!

To begin, please download the executable files first. You will need a Linux environment to run them since they are not in the .exe format.

### Application Download

There are two pairs of client-server applications. One is implemented using TCP sockets and the other using UDP sockets. Please use them accordingly since, for example, a UDP client cannot work with a running TCP server.

Here are the TCP server: [TCP Server](https://github.com/WilliamShen02/udp-tcp-chat/raw/main/executable/chatserver_tcp) and the TCP client [TCP Client](https://github.com/WilliamShen02/udp-tcp-chat/raw/main/executable/chatclient_tcp) applications.  
Here are the UDP server: [UDP Server](https://github.com/WilliamShen02/udp-tcp-chat/raw/main/executable/chatserver_udp) and the UDP client (https://github.com/WilliamShen02/udp-tcp-chat/raw/main/executable/chatclient_udp) applications.

After downloading the executable files, please look at the instructions for use as follows.

### Instructions for Use

#### 1. Server setup and setting password

First open a command line prompt in the directory where the server executable file is located.

Type in the following command and fill in the blanks marked with "\*". Make sure you type the correct file name (tcp or udp). Recommended port is 5001, and there is no passcode limitations.

```
Command: ./chatserver_*** -start -port **** -passcode ****  
Output: Server started on port <port>. Accepting connections
```

#### 2. Client setup and password checking

Open another command line prompt in the directory where the client executable file is located.  
Type in the following command and fill in the blanks marked with "\*".

```
Command: ./chatclient -join -host 127.0.0.1 -port **** -username **** -passcode ****
Output (on Server): <username> joined the chatroom
Output (on new Client): Connected to <hostname> on port <port>
```

If the password is incorrect, the prompt will output `Incorrect Passcode` and return. Please make sure it is correct.

You may open other prompts as other clients since the server supports multiple clients connected to it, but the usernames inputted must be different. 

If you type `listclients` on the server prompt, it will output the usernames of all connected clients.

#### 3. Chatting

After successfully connected to the server, clients will be able to type messages that get sent to the server when the user enters a newline. All text before the newline will be sent to the server, displayed on the server's screen, and broadcasted and displayed on the screens of all clients.

```
Command (on a connected client with username: <username>): Hello Room  
Output (on Server): <username>: Hello Room  
Output (on all other clients): <username>: Hello Room
```

Please limit your message to 100 characters.

#### 4. Chatting Shortcuts

Clients will be able to send message shortcuts to the server that are translated into text and displayed as text at all screens.

For example,  
```
Command (on a connected client): :)
Output (on Server): <username>: [Feeling Happy]
Output (on all other clients): <username>: [Feeling Happy]
```

Here is the list of all message shortcuts:  
1. Type `:)` to display `[feeling happy]`  
2. Type `:(` to display `[feeling sad]`  
3. Type `:mytime` to display the current time  
4. Type `:+1hr` to display the current time + 1 hour  

#### 5. Chatting Termination

Clients will be able to disconnect by typing `:Exit`, like this:  
```
Command (on a connected client): :Exit
Output (on Server): <username> left the chatroom
Output (on all other clients): <username> left the chatroom
```

### Source Code

This project is implemented using C/C++, and it can be compiled by both GCC and G++. The source code could be found here: [Source Code](https://github.com/WilliamShen02/udp-tcp-chat/tree/main/source_code)
