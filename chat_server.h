#include "chat_message.h"

#ifndef SUPER_INVENTION_CHAT__CHAT_SERVER_H_
#define SUPER_INVENTION_CHAT__CHAT_SERVER_H_

/* Thread routine to serve connection to client. */
void process_client_message(int server_socket_fd, struct sockaddr_in *client_address, chat_message_t *message);

/* Signal handler to handle SIGTERM and SIGINT signals. */
void signal_handler(int signal_number);

void send_ack(int socket, struct sockaddr_in *client_address, short message_id);
void send_nack(int socket, struct sockaddr_in *client_address, short message_id);

void SetupSignalHandler();

int CreateServerSocket(int port);

#endif //SUPER_INVENTION_CHAT__CHAT_SERVER_H_
