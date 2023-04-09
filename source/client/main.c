#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include "../libs/Utils.h"
#include "../libs/Macro.h"

typedef void (*Handler)(char *);

typedef struct {
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    Handler handler;
    bool (*checkInputParameters)(int argc, char **argv);
    void (*setupServerAddr)(char **argv);
    bool (*bindClientAddr)(void);
    char* (*keyinMessage)(void);
    int (*retryEchoMessage)(char *message);
} UDPSocketSubject;

bool checkInputParameters(int argc, char **argv);
void setupServerAddr(char **argv);
bool bindClientAddr(void);
char* keyinMessage(void);
int retryEchoMessage(char *message);
void printMessage(char *message);
int retryTimeout(int base_million_sec, int multiplier, int failure_times, int max_million_sec);

UDPSocketSubject udpSocketSubject = {
    .checkInputParameters = checkInputParameters,
    .setupServerAddr = setupServerAddr,
    .bindClientAddr = bindClientAddr,
    .keyinMessage = keyinMessage,
    .retryEchoMessage = retryEchoMessage,
};

int main(int argc, char **argv) {
    print_client_log("Client start with version : 0.1");
    udpSocketSubject.handler = printMessage;

    if (!udpSocketSubject.checkInputParameters(argc, argv)) {
        print_client_log("Please check arguments!\nUsage : %s IP PORT", argv[0]);
        return RESULT_ERROR;
    }

    udpSocketSubject.setupServerAddr(argv);

    if (!udpSocketSubject.bindClientAddr()) {
        return RESULT_ERROR;
    }

    char *message = udpSocketSubject.keyinMessage();
    int result = udpSocketSubject.retryEchoMessage(message);
    
    switch (result)
    {
    case RESULT_SUCCESS:
        return 0;
        break;
    case RESULT_MESSAGE_RETRY_REACH_MAX:
        return 1;
        break;
    }
}

bool checkInputParameters(int argc, char **argv) {
    if (argc < 3) {
        return false;
    }

    return true;
}

void setupServerAddr(char **argv) {
    // setup udp socket ip and port
    int port = string_to_int(argv[2]);
    char *ip = argv[1];

    print_client_log("Setup server udp socket with %s:%d", ip, port);
    udpSocketSubject.server_addr.sin_family = AF_INET;
    udpSocketSubject.server_addr.sin_port = htons(port);
    udpSocketSubject.server_addr.sin_addr.s_addr = inet_addr(ip);
}

bool bindClientAddr(void) {
    udpSocketSubject.client_addr.sin_family = AF_INET;
    udpSocketSubject.client_addr.sin_addr.s_addr = INADDR_ANY;
    udpSocketSubject.client_addr.sin_port = htons(0);

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
    int bind_result = bind(socket_fd, (struct sockaddr *) &udpSocketSubject.client_addr, sizeof(udpSocketSubject.client_addr));

    // bind udp socket error
    if( bind_result < 0) {
        return false;
    }

    print_client_log("Binding udp socket success!");
    return true;
}

char* keyinMessage(void) {
    char message[MESSAGE_BUFFER_SIZE];
    memset(message, '\0', sizeof(message));

    print_client_log("Please enter message: ");
    return fgets(message, sizeof(message), stdin);
}

int retryEchoMessage(char *message) {
    char recv_message[MESSAGE_BUFFER_SIZE];
    unsigned int server_addr_length = sizeof(udpSocketSubject.server_addr);
    int failure_times = 0;

    do {
        print_client_log("Sending message...");
        sendto(udpSocketSubject.socket_fd, message, strlen(message), 0, (struct sockaddr*)&udpSocketSubject.server_addr, server_addr_length);

        // Setup receive timeout million sec
        int timeout_ms = retryTimeout(RETRY_BASE_MS, RETRY_MULTIPIER, failure_times, RETRY_MAX_WAIT_MS);
        struct timeval time_val;
        time_val.tv_sec = timeout_ms / 1000;
        time_val.tv_usec = timeout_ms % 1000 * 1000; // 1 million = 1 * 1000 usec
        print_client_log("Setup receive timeout : %d ms", timeout_ms);
        setsockopt(udpSocketSubject.socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time_val, sizeof(time_val));

        print_client_log("Wait echo message...");
        memset(recv_message, '\0', sizeof(recv_message));
        int result = recvfrom(
            udpSocketSubject.socket_fd, 
            recv_message, 
            sizeof(recv_message), 
            0, 
            (struct sockaddr *)&udpSocketSubject.server_addr, 
            &server_addr_length
        );

        if (result != RESULT_ERROR) {
            // Send success and receive echo, notify to observer handle it.
            udpSocketSubject.handler(recv_message);
            break;
        } else if (failure_times == RETRY_MAX_TIMES) {
            print_client_log("Retry reach max times %d, stop retry!", failure_times);
            break;
        }

        failure_times++;
        print_client_log("Timeout! retry times : %d", failure_times);
    } while(true);

    // Return result by each case
    if (failure_times == RETRY_MAX_TIMES) { 
        return RESULT_MESSAGE_RETRY_REACH_MAX;
    } else {
        return RESULT_SUCCESS;
    }
}

void printMessage(char *message) {
    print_client_log("Receive message : %s", message);
}

int retryTimeout(int base_million_sec, int multiplier, int failure_times, int max_million_sec) {
    int timeout = 0;
    int result = 1;

    // Caculate timeout : base * (multiplier ^ failure_times)
    for (int i = 0 ; i < failure_times ; i++) {
        result = result * multiplier;
    }
    timeout = base_million_sec * result;

    // Check max timeout
    if (timeout > max_million_sec) {
        return max_million_sec;
    }

    return timeout;
}