//
// Created by 310165137 on 18/05/2022.
//

#ifndef SUPER_INVENTION_CHAT__CHAT_CLIENT_H_
#define SUPER_INVENTION_CHAT__CHAT_CLIENT_H_

void process_send_request(int socket_fd,  struct sockaddr_in *server_address, char *client_id_ascii, char *message);
void process_receive_request(int socket_fd,  struct sockaddr_in *server_address,  char *client_id_ascii);

#endif //SUPER_INVENTION_CHAT__CHAT_CLIENT_H_
