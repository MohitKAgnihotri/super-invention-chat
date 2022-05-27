#ifndef SUPER_INVENTION_CHAT__USER_INTERFACE_H_
#define SUPER_INVENTION_CHAT__USER_INTERFACE_H_

#define MAX_INPUT_LEN 255

void print_user_menu ( void );
int get_user_choice( void);
void get_line_from_stdin(char *line, int max_length);
void Get_Client_ID(char *string, int len);

#endif //SUPER_INVENTION_CHAT__USER_INTERFACE_H_
