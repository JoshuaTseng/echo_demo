#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../libs/Utils.h"

/**
 * underlying IPv4 protocol, is 65,507 bytes 
 * (65,535 bytes − 8-byte UDP header − 20-byte IP header)
 * ref : https://en.wikipedia.org/wiki/User_Datagram_Protocol
*/
#define MESSAGE_BUFFER_SIZE 65507
#define print_log(format_str, ...) printf("[%s] ", timestamp()), printf((format_str), ##__VA_ARGS__), printf("\n")

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
    print_log("Start receive message...");
    int client_addr_length = sizeof(client_addr);
    bool exit = false;
    while (!exit) {
        // if error result is -1, otherwise result is data length
        print_log("Wait receive message...");
        int result = recvfrom(socket_fd, recv_message, sizeof(recv_message), 0,
         (struct sockaddr *) &client_addr, &client_addr_length);
        print_log("result message length : %d", result);
        print_log("message : %s", recv_message);
        
        // Get my ip address and port
        bzero(&client_addr, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);
        getsockname(socket_fd, (struct sockaddr *) &client_addr, &len);
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_addr, sizeof(ip_addr));
        port_no = ntohs(client_addr.sin_port);
        print_log("address : %s, port %d", ip_addr, port_no);
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
