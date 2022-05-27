#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include "chat_server.h"
#include "chat_message.h"

client_t client_list[MAX_CLIENT_NUMBER];
int server_socket_fd;

void init_client_list() {
  int i;
  for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
    memset(client_list[i].client_id, 0, MAX_CLIENT_ID_LEN);
    client_list[i].is_logged_in = false;
    for (int j = 0; j < MAX_MESSAGES; j++) {
      client_list[i].outstanding_messages[j] = NULL;
    }
  }
}

bool is_client_logged_in(char *client_id_ascii) {
  int i;
  bool is_logged_in = false;
  for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
    if (strcmp(client_list[i].client_id, client_id_ascii) == 0 && client_list[i].is_logged_in == true) {
      is_logged_in = true;
      break;
    }
  }
  return is_logged_in;
}

int get_free_client_index() {
  int i;
  for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
    if (client_list[i].is_logged_in == false) {
      return i;
    }
  }
  return -1;
}

int get_client_index(char *client_id_ascii) {
  int i;
  for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
    if (strcmp(client_list[i].client_id, client_id_ascii) == 0 && client_list[i].is_logged_in == true) {
      return i;
    }
  }
  return -1;
}

void set_client_logged_in(int index, char *client_id_ascii) {
  strcpy(client_list[index].client_id, client_id_ascii);
  client_list[index].is_logged_in = true;
}

void reset_client_logged_in(int index) {
  memset(client_list[index].client_id, 0, MAX_CLIENT_ID_LEN);
  client_list[index].is_logged_in = false;
  for (int i = 0; i < MAX_MESSAGES; i++) {
    if (client_list[index].outstanding_messages[i] != NULL) {
      free(client_list[index].outstanding_messages[i]);
      client_list[index].outstanding_messages[i] = NULL;
    }
  }
}

void store_message(int index, char *message) {
  int i;
  for (i = 0; i < MAX_MESSAGES; i++) {
    if (client_list[index].outstanding_messages[i] == NULL) {
      client_list[index].outstanding_messages[i] = (char *) malloc(MAX_MESSAGE_LEN);
      strcpy(client_list[index].outstanding_messages[i], message);
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  int port, new_socket_fd;
  pthread_attr_t pthread_client_attr;
  pthread_t pthread;
  socklen_t client_address_len;
  struct sockaddr_in client_address;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(1);
  }

  /* Get port from command line arguments or stdin.
   * For this server, this is fixed to 1113*/
  port = atoi(argv[1]);

  /*Create the server socket */
  server_socket_fd = CreateServerSocket(port);

  /*Setup the signal handler*/
  SetupSignalHandler();

  /* Initialise pthread attribute to create detached threads. */
  if (pthread_attr_init(&pthread_client_attr) != 0) {
    perror("pthread_attr_init");
    exit(1);
  }
  if (pthread_attr_setdetachstate(&pthread_client_attr, PTHREAD_CREATE_DETACHED) != 0) {
    perror("pthread_attr_setdetachstate");
    exit(1);
  }

  while (1) {
    chat_message_t message;
    client_address_len = sizeof client_address;
    if (recvfrom(server_socket_fd,
                 &message,
                 sizeof message,
                 0,
                 (struct sockaddr *) &client_address,
                 &client_address_len) < 0) {
      perror("recvfrom");
      exit(1);
    }

    process_client_message(server_socket_fd, &client_address, &message);
  }

}

int CreateServerSocket(int port) {
  struct sockaddr_in address;
  int socket_fd;

  /* Initialise IPv4 address. */
  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  /* Create UDP socket. */
  if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  /* Bind address to socket. */
  if (bind(socket_fd, (struct sockaddr *) &address, sizeof(address)) == -1) {
    perror("bind");
    exit(1);
  }

  // Configure server socket
  int enable = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
  return socket_fd;
}

void SetupSignalHandler() {/* Assign signal handlers to signals. */
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    perror("signal");
    exit(1);
  }
  if (signal(SIGTERM, signal_handler) == SIG_ERR) {
    perror("signal");
    exit(1);
  }
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    perror("signal");
    exit(1);
  }
}

int send_message_for_client(int socket, struct sockaddr_in *client_address, int client_index, short message_id) {
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = MESSAGE;
  message.message_id = message_id;
  message.payload_length = sizeof(payload_t);

  strncpy(message.payload.message.client_id.message, client_list[client_index].client_id, MAX_CLIENT_ID_LEN);
  for (int i = 0; i < MAX_MESSAGES; i++) {
    if (client_list[client_index].outstanding_messages[i] != NULL) {
      strncpy(message.payload.message.message.message,
              client_list[client_index].outstanding_messages[i],
              MAX_MESSAGE_LEN);
      free(client_list[client_index].outstanding_messages[i]);
      client_list[client_index].outstanding_messages[i] = NULL;
      break;
    }
  }

  if (sendto(socket,
             &message,
             sizeof message,
             0,
             (struct sockaddr *) client_address,
             sizeof *client_address) < 0) {
    perror("sendto");
    exit(1);
  }
  return 0;

}

void process_client_message(int socket, struct sockaddr_in *client_address, chat_message_t *message) {
  if (message->version != MESSAGE_PROTOCOL_VERSION) {
    send_nack(socket, client_address, message->message_id);
    return;
  }

  if (message->type == LOGIN) {
    send_ack(socket, client_address, message->message_id);
    if (!is_client_logged_in(message->payload.login.client_id.message)) {
      int free_index = get_free_client_index();
      if (free_index != -1) {
        set_client_logged_in(free_index, message->payload.login.client_id.message);
//          send_login_success(socket, client_address, message->payload.login.client_id.message);
      } else {
//        send_login_failure(socket, client_address, message->payload.login.client_id.message);
      }
      return;
    }
  } else if (message->type == LOGOUT) {
    send_ack(socket, client_address, message->message_id);
    if (is_client_logged_in(message->payload.logout.client_id.message)) {
      int client_index = get_client_index(message->payload.logout.client_id.message);
      reset_client_logged_in(client_index);
    }
  } else if (message->type == MESSAGE) {
    send_ack(socket, client_address, message->message_id);
    if (is_client_logged_in(message->payload.message.client_id.message)) {
      int client_index = get_client_index(message->payload.message.client_id.message);

      /*Send message */
      if (strlen(message->payload.message.client_id.message) > 0
          && strlen(message->payload.message.message.message) == 0) {
        send_message_for_client(socket, client_address, client_index, message->message_id);
      }

      /*Store Message*/
      if (strlen(message->payload.message.client_id.message) > 0
          && strlen(message->payload.message.message.message) > 0) {
        store_message(client_index, message->payload.message.message.message);
      }
    }
  } else {
    send_nack(socket, client_address, message->message_id);
  }

}

void send_ack(int socket, struct sockaddr_in *client_address, short message_id) {
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = ACK;
  message.message_id = message_id;
  message.payload_length = 0;

  if (sendto(socket,
             &message,
             sizeof message,
             0,
             (struct sockaddr *) client_address,
             sizeof *client_address) < 0) {
    perror("sendto");
    exit(1);
  }
}

void send_nack(int socket, struct sockaddr_in *client_address, short message_id) {
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = NAK;
  message.message_id = message_id;
  message.payload_length = 0;

  if (sendto(socket,
             &message,
             sizeof message,
             0,
             (struct sockaddr *) client_address,
             sizeof *client_address) < 0) {
    perror("sendto");
    exit(1);
  }
}

void signal_handler(int signal_number) {
  close(server_socket_fd);
  exit(0);
}