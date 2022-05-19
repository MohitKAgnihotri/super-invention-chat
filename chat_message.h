
#ifndef SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_
#define SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_


#define MAX_CLIENT_ID 255
#define MAX_MESSAGE_LEN 1024

typedef struct chat_message {
  char version;
  char type;
  short message_id;
  unsigned int payload_length;
  char payload[0];
} chat_message_t;

typedef enum message_type {
  LOGIN = 0x01,
  MESSAGE = 0x02,
  ACK = 0x03,
  NAK = 0x04,
  LOGOUT = 0x05
} message_type_t;

typedef struct string_payload
{
  unsigned short length;
  char message[MAX_MESSAGE_LEN];
}string_payload_t;

typedef struct payload_login
{
  char client_id[MAX_CLIENT_ID];
}payload_login_t;

typedef struct payload_logout
{
  char client_id[MAX_CLIENT_ID];
}payload_logout_t;

typedef struct payload_message {
  char client_id[MAX_CLIENT_ID];
  string_payload_t message[MAX_MESSAGE_LEN];
} payload_message_t;

#endif //SUPER_INVENTION_CHAT__CHAT_MESSAGE_H_
