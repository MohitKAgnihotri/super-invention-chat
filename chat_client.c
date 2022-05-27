#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include "user_interface.h"
#include "chat_client.h"
#include "chat_message.h"

#define SERVER_NAME_LEN_MAX 255

void process_login_request(int socket_fd,  struct sockaddr_in *server_address, char *client_id_ascii)
{
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = LOGIN;
  message.message_id = random() % 65535;
  message.payload_length = sizeof(payload_login_t);
  strcpy(message.payload.login.client_id.message, client_id_ascii);

  if (sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
  {
    perror("sendto");
    exit(1);
  }

  chat_message_t response;
  memset(&response, 0, sizeof(response));
  if (recvfrom(socket_fd, &response, sizeof(response), 0, NULL, NULL) < 0)
  {
      perror("recvfrom");
      exit(1);
  }

  if (response.type == ACK)
  {
      printf("Login successful\n");
  }
  else if (response.type == NAK)
  {
      printf("Login failed\n");
  }
  else
  {
      printf("Unexpected response\n");
  }
}

void process_logout_request(int socket_fd,  struct sockaddr_in *server_address, char *client_id_ascii)
{
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = LOGOUT;
  message.message_id = random() % 65535;
  message.payload_length = sizeof(payload_login_t);
  strcpy(message.payload.logout.client_id.message, client_id_ascii);

  if (sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
  {
    perror("sendto");
    exit(1);
  }

  chat_message_t response;
  memset(&response, 0, sizeof(response));
  if (recvfrom(socket_fd, &response, sizeof(response), 0, NULL, NULL) < 0)
  {
    perror("recvfrom");
    exit(1);
  }

  if (response.type == ACK)
  {
    printf("Logout successful\n");
  }
  else if (response.type == NAK)
  {
    printf("Logout failed\n");
  }
  else
  {
    printf("Unexpected response\n");
  }
}


void process_send_request(int socket_fd,  struct sockaddr_in *server_address, char *client_id_ascii, char *message_to_be_sent)
{
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = MESSAGE;
  message.message_id = random() % 65535;
  message.payload_length = sizeof(payload_login_t);
  strcpy(message.payload.message.client_id.message, client_id_ascii);
  strcpy(message.payload.message.message.message, message_to_be_sent);

  if (sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
  {
    perror("sendto");
    exit(1);
  }

  chat_message_t response;
  memset(&response, 0, sizeof(response));
  if (recvfrom(socket_fd, &response, sizeof(response), 0, NULL, NULL) < 0)
  {
    perror("recvfrom");
    exit(1);
  }

  if (response.type == ACK)
  {
    printf("Message Transmitted successful\n");
  }
  else if (response.type == NAK)
  {
    printf("Message Transmission Failed\n");
  }
  else
  {
    printf("Unexpected response\n");
  }
}

void process_receive_request(int socket_fd,  struct sockaddr_in *server_address,  char *client_id_ascii)
{
  chat_message_t message;
  memset(&message, 0, sizeof(message));

  message.version = MESSAGE_PROTOCOL_VERSION;
  message.type = MESSAGE;
  message.message_id = random() % 65535;
  message.payload_length = sizeof(payload_login_t);
  strcpy(message.payload.message.client_id.message, client_id_ascii);

  if (sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr *)server_address, sizeof(*server_address)) < 0)
  {
    perror("sendto");
    exit(1);
  }

  chat_message_t response;
  memset(&response, 0, sizeof(response));
  if (recvfrom(socket_fd, &response, sizeof(response), 0, NULL, NULL) < 0)
  {
    perror("recvfrom");
    exit(1);
  }

  if (response.type == ACK)
  {
    printf("Message Transmitted successful\n");

    chat_message_t message_received;
    memset(&message_received, 0, sizeof(message_received));
    if (recvfrom(socket_fd, &message_received, sizeof(message_received), 0, NULL, NULL) < 0)
    {
      perror("recvfrom");
      exit(1);
    }
    printf("Message Received: %s\n", message_received.payload.message.message.message);
  }
  else if (response.type == NAK)
  {
    printf("Message Transmission Failed\n");
  }
  else
  {
    printf("Unexpected response\n");
  }
}

int main(int argc, char *argv[])
{
  char server_name[SERVER_NAME_LEN_MAX + 1] = { 0 };
  int server_port, socket_fd;
  struct hostent *server_host;
  struct sockaddr_in server_address;
  bool is_exit_requested = false;

  /* Get server name from command line arguments or stdin. */
  if (argc > 1)
  {
    strncpy(server_name, argv[1], SERVER_NAME_LEN_MAX);
  }
  else
  {
    printf("Enter Server Name: ");
    scanf("%s", server_name);
  }

  /* Get server port from command line arguments or stdin. */
  server_port = argc > 2 ? atoi(argv[2]) : 0;
  if (!server_port)
  {
    printf("Enter Port: ");
    scanf("%d", &server_port);
  }

  /* Get server host from server name. */
  server_host = gethostbyname(server_name);

  /* Initialise IPv4 server address with server host. */
  memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(server_port);
  memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

  /* Create UDP socket. */
  if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  while (!is_exit_requested)
  {
    print_user_menu();
    int choice = get_user_choice();
    switch (choice) {
      case 1:
      {
        char client_id_ascii[MAX_INPUT_LEN] = { 0 };
        get_client_id(client_id_ascii, MAX_INPUT_LEN);
        process_login_request(socket_fd, &server_address, client_id_ascii);
      }
      break;
      case 2:
      {
        char client_id_ascii[MAX_INPUT_LEN] = { 0 };
        get_client_id(client_id_ascii, MAX_INPUT_LEN);
        process_logout_request(socket_fd, &server_address, client_id_ascii);
      }
      break;
      case 3:
      {
        char client_id_ascii[MAX_INPUT_LEN] = { 0 };
        get_client_id(client_id_ascii, MAX_INPUT_LEN);
        char message_ascii[MAX_INPUT_LEN] = { 0 };
        get_message_to_be_sent(message_ascii, MAX_INPUT_LEN);
        process_send_request(socket_fd, &server_address, client_id_ascii, message_ascii);
      }
      break;
      case 4:
      {
        char client_id_ascii[MAX_INPUT_LEN] = { 0 };
        get_client_id(client_id_ascii, MAX_INPUT_LEN);
        process_receive_request(socket_fd, &server_address, client_id_ascii);
      }
      break;
      case 5:
      {
        is_exit_requested = true;
      }
      default:
        printf("Invalid choice\n");
        break;
    }
  }
  close(socket_fd);
  return 0;
}
