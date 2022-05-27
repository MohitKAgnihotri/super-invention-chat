
#ifndef SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_
#define SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_


#define MAX_CLIENT_ID_LEN 255
#define MAX_MESSAGE_LEN 255
#define MAX_MESSAGES 100
#define MAX_CLIENT_NUMBER 100

#define MESSAGE_PROTOCOL_VERSION 1


typedef struct client
{
  bool is_logged_in;
  int sent_index;
  char client_id[MAX_CLIENT_ID_LEN];
  char *outstanding_messages[MAX_MESSAGES];
} client_t;

typedef struct string_payload
{
  unsigned short length;
  char message[MAX_MESSAGE_LEN];
}string_payload_t;

typedef struct payload_login
{
  string_payload_t client_id;
}payload_login_t;

typedef struct payload_logout
{
  string_payload_t client_id;
}payload_logout_t;

typedef struct payload_message {
  string_payload_t client_id;
  string_payload_t message;
} payload_message_t;

typedef union payload{
  payload_login_t login;
  payload_logout_t logout;
  payload_message_t message;
}payload_t;

typedef struct chat_message {
  char version;
  char type;
  short message_id;
  unsigned int payload_length;
  payload_t payload;
} chat_message_t;

typedef enum message_type {
  LOGIN = 0x01,
  MESSAGE = 0x02,
  ACK = 0x03,
  NAK = 0x04,
  LOGOUT = 0x05
} message_type_t;



#endif //SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_
