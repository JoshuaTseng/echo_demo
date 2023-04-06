#ifndef _MACRO_H_
#define _MACRO_H_

/**
 * underlying IPv4 protocol, is 65,507 bytes 
 * (65,535 bytes − 8-byte UDP header − 20-byte IP header)
 * ref : https://en.wikipedia.org/wiki/User_Datagram_Protocol
*/
#define MESSAGE_BUFFER_SIZE 65507
#define print_log(format_str, ...) printf("[%s] ", timestamp()), printf((format_str), ##__VA_ARGS__), printf("\n")

#endif /* !_MACRO_H_ */
