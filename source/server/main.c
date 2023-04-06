#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include "../libs/Utils.h"
#include "../libs/Macro.h"

// User input
bool is_requirement_args_exist(int argc, char **argv);
// Socket manage
bool is_socket_fd_open_success(int socket_fd);

int main(int argc, char **argv) {
    int socket_fd;
    int port_no;
    char *ip_addr;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char recv_message[MESSAGE_BUFFER_SIZE];

    if (!is_requirement_args_exist(argc, argv)) {
        print_log("Please check arguments!\nUsage : %s IP PORT", argv[0]);
        return -1;
    }

    // Clean buffers:
    memset(recv_message, '\0', sizeof(recv_message));

    /**
     * Create socket with option :
     * domain IPv4, type UDP, protocol UDP
    */
    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (!is_socket_fd_open_success(socket_fd)) {
        print_log("Socket open error, please check ethernet, port is available!");
        return -1;
    }

    // Load server config
    print_log("Load server config...");
    port_no = string_to_int(argv[2]);
    ip_addr = argv[1];
    print_log("IP %s, port %d", ip_addr, port_no);

    // Bind address and port:
    print_log("Start binding...");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr);
    if(bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        print_log("Couldn't bind to the ip and port");
        return -1;
    }
    print_log("Bind socket successful!");

    // Wait receive message
    unsigned int client_addr_length = sizeof(client_addr);
    bool exit = false;
    int result = 0;
    while (!exit) {
        // if error result is -1, otherwise result is data length
        print_log("Wait receive message...");
        result = recvfrom(socket_fd, recv_message, sizeof(recv_message), 0, (struct sockaddr *) &client_addr, &client_addr_length);
        print_log("Received message form %s : %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
        print_log("Length : %d\nmessage : %s", result, recv_message);
        // TODO: error handle

        // Echo message
        result = sendto(socket_fd, recv_message, strlen(recv_message), 0, (struct sockaddr *) &client_addr, client_addr_length);
        print_log("Echo message success!");
        // TODO: error handle
    }

}

bool is_requirement_args_exist(int argc, char **argv) {
    if (argc < 3) {
        return false;
    }

    return true;
}

bool is_socket_fd_open_success(int socket_fd) {
    if (socket_fd < 0) {
        return false;
    }

    return true;
}
