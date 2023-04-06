#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include "../libs/Utils.h"
#include "../libs/Macro.h"

int main(int argc, char **argv) {
    int socket_fd, port_no;
    char *ip_addr;
    struct sockaddr_in server_addr, client_addr;
    char recv_message[MESSAGE_BUFFER_SIZE], send_message[MESSAGE_BUFFER_SIZE];

    port_no = string_to_int(argv[2]);
    ip_addr = argv[1];

    // Clean buffers:
    memset(recv_message, '\0', sizeof(recv_message));
    memset(send_message, '\0', sizeof(send_message));

    // Create socket:
    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // Set server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);

    // Set client
    client_addr.sin_family = AF_INET;
    // client_addr.sin_addr.s_addr = inet_addr(ip_addr);
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(0);

    // client bind
    if(bind(socket_fd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        print_log("Couldn't bind to the ip and port");
        return -1;
    }
    
    // Get input from the user:
    print_log("Enter message: ");
    fgets(send_message, sizeof(send_message), stdin);
    
    // Send the message to server:
    unsigned int server_addr_length = sizeof(server_addr);
    if(sendto(socket_fd, send_message, strlen(send_message), 0, (struct sockaddr*)&server_addr, server_addr_length) < 0){
        print_log("Unable to send message\n");
        return -1;
    }
    
    // Receive the server's response:
    if(recvfrom(socket_fd, recv_message, sizeof(recv_message), 0, (struct sockaddr *)&server_addr, &server_addr_length) < 0){
        print_log("Error while receiving server's msg\n");
        return -1;
    }
    
    print_log("Server's response: %s\n", recv_message);
    return 0;
}