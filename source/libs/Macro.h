#ifndef _MACRO_H_
#define _MACRO_H_

/**
 * underlying IPv4 protocol, is 65,507 bytes 
 * (65,535 bytes − 8-byte UDP header − 20-byte IP header)
 * ref : https://en.wikipedia.org/wiki/User_Datagram_Protocol
*/
#define MESSAGE_BUFFER_SIZE 10000

#define RESULT_ERROR -1
#define RESULT_SUCCESS 0
#define RESULT_MESSAGE_RETRY_REACH_MAX 1

// UDP Socket retry config
#define RETRY_BASE_MS 500
#define RETRY_MULTIPIER 2
#define RETRY_MAX_TIMES 10
#define RETRY_MAX_WAIT_MS 8*1000

#define print_server_log(format_str, ...) printf("[%s][Server] ", timestamp()), printf((format_str), ##__VA_ARGS__), printf("\n")
#define print_client_log(format_str, ...) printf("[%s][Client] ", timestamp()), printf((format_str), ##__VA_ARGS__), printf("\n")

#endif /* !_MACRO_H_ */
