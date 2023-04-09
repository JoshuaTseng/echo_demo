#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include "../libs/Utils.h"
#include "../libs/Macro.h"

typedef void (*Handler)(char *);
typedef void (*Notify)(void);

typedef struct {
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    Handler handler;
    bool (*checkInputParameters)(int argc, char **argv);
    bool (*bindWithArgv)(char **argv);
    void (*receiveMessage)(void);
} UDPSocketSubject;

bool checkInputParameters(int argc, char **argv);
bool bindUDPSocketWithArgv(char **argv);
void receiveMessage(void);
void echoMessage(char *message);

UDPSocketSubject udpSocketSubject = {
    .checkInputParameters = checkInputParameters,
    .bindWithArgv = bindUDPSocketWithArgv,
    .receiveMessage = receiveMessage
};

int main(int argc, char **argv) {
    print_server_log("Server start with version : 0.1");
    udpSocketSubject.handler = echoMessage;

    if (!udpSocketSubject.checkInputParameters(argc, argv)) {
        print_server_log("Please check arguments!\nUsage : %s IP PORT", argv[0]);
        return RESULT_ERROR;
    }

    if (!udpSocketSubject.bindWithArgv(argv)) {
        return RESULT_ERROR;
    }

    udpSocketSubject.receiveMessage();

    return 0;
}

bool checkInputParameters(int argc, char **argv) {
    if (argc < 3) {
        return false;
    }

    return true;
}

bool bindUDPSocketWithArgv(char **argv) {
    // setup udp socket ip and port
    int port = string_to_int(argv[2]);
    char *ip = argv[1];

    print_server_log("Binding udp socket with %s:%d", ip, port);
    udpSocketSubject.server_addr.sin_family = AF_INET;
    udpSocketSubject.server_addr.sin_port = htons(port);
    udpSocketSubject.server_addr.sin_addr.s_addr = inet_addr(ip);

    /**
     * Create socket with option :
     * domain IPv4, type UDP, protocol UDP
    */
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // open socket file description error
    if (socket_fd < 0) {
        return false;
    }

    // update socket file description, for socket control
    udpSocketSubject.socket_fd = socket_fd;

    // bind udp socket
    int bind_result = bind(socket_fd, (struct sockaddr *) &udpSocketSubject.server_addr, sizeof(udpSocketSubject.server_addr));

    // bind udp socket error
    if( bind_result < 0) {
        return false;
    }

    print_server_log("Binding udp socket success!");
    return true;
}

void receiveMessage(void) {
    unsigned int client_addr_length = sizeof(udpSocketSubject.client_addr);
    bool exit = false;
    int result = 0;
    char message[MESSAGE_BUFFER_SIZE];

    while (!exit) {
        // clean message buffer
        memset(message, '\0', sizeof(message));

        // block current thread, wait until any message arrived
        print_server_log("Wait message arrive...");
        result = recvfrom(
            udpSocketSubject.socket_fd, 
            message, 
            sizeof(message), 
            0, 
            (struct sockaddr *) &udpSocketSubject.client_addr, 
            &client_addr_length
        );

        // receive message error, exit
        if (result < 0) {
            print_server_log("Receive message error, exit");
            exit = true;
        }

        print_server_log("Got message, handling...");
        udpSocketSubject.handler(message);
    }
}

void echoMessage(char *message) {
    unsigned int client_addr_length = sizeof(udpSocketSubject.client_addr);
    int result = 0;

    print_server_log("Sending echo message...");
    result = sendto(
        udpSocketSubject.socket_fd, 
        message, 
        strlen(message), 
        0, 
        (struct sockaddr *) &udpSocketSubject.client_addr, 
        client_addr_length
    );

    // receive message error, exit
    if (result < 0) {
        print_server_log("Send message error, ignore it...");
    }

    print_server_log("Sent echo message success!");
}