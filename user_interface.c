#include <stdio.h>
#include <stdlib.h>
#include "user_interface.h"

void get_line_from_stdin(char *line, int max_length) {
  int i = 0;
  char c;
  while (i < max_length - 1 && (c = getchar()) != '\n') {
    line[i] = c;
    i++;
  }
  line[i] = '\0';
}

void Get_Client_ID(char *string, int len) {
  printf("Enter Client-ID: \n");
  get_line_from_stdin(string, len);
}

void print_user_menu(void) {
  printf("Welcome to the Chat client\n");
  printf("Please select one of the following options:\n");
  printf("1. Login\n");
  printf("2. Logout\n");
  printf("3. Send\n");
  printf("4. Receive\n");
}

int get_user_choice(void) {
  char choice;
  int choice_int;
  printf("Please enter your choice: ");
  scanf("%c", &choice);
  choice_int = atoi(&choice);
  (void) getc(stdin); //flush \n
  return choice_int;
}
