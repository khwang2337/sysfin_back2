#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "networking.h"

int checkSYM(char * word) {
  strsep(&word, " !@#$^&*()_-+={[}]|\\:;\"\'<>?,./~`%");
  return word == 0;
}

int opening(int sd) {
  char buffer[MESSAGE_BUFFER_SIZE];
  
  int check = 1;
  while (check) { //ask client if they have an account
    printf("Do you already have an account? Enter yes or no: ");
    fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
    if ( buffer[strlen(buffer) - 1] != '\n' ) { //checks for out of bounds
        printf("Error: Out of bounds; character limit is %d\n", MESSAGE_BUFFER_SIZE - 1); 
        int clear;
        while ( (clear = getchar()) != '\n' && clear != EOF); //clears stdin
    }
    else {
      buffer[ strlen(buffer) - 1 ] = 0; //replaces newline
      if ( (! strcmp(buffer,"yes")) || (! strcmp(buffer,"no")) ) {
        write(sd, buffer, strlen(buffer));
        check = 0;
      }
    }
  }
  if (! strcmp(buffer, "yes")) return -1;
  else return 1;
}

void registerC(int sd) {
  char buffer[MESSAGE_BUFFER_SIZE] = {0};
  int check = 1;
  
  while (check > 0) {
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    if (! strcmp(buffer, "makeuser")) {
      while (check) {
        if (check == 2) printf("Too Short! Must be more than 5 characters!\n");
        if (check == 3) printf("Invalid symbol used!\n");
        printf("Preferred username: ");
        fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
        buffer[ strlen(buffer) - 1] = 0;
        if (strlen(buffer) <= 5) check = 2;
        else if (! checkSYM(buffer)) check = 3;
        else {
          write(sd, buffer, MESSAGE_BUFFER_SIZE);
          check = 0;
        }
      }
      check = 1;
    }
    else if (! strcmp(buffer, "makepass")) {
      while (check) {
        if (check == 2) printf("Too Short! Must be more than 5 characters!\n");
        if (check == 3) printf("Invalid symbol used!\n");
        printf("Preferred password?: ");
        fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
        if (strlen(buffer) <= 6) check = 2;
        else if (! checkSYM(buffer)) check = 3;
        else {
          write(sd, buffer, MESSAGE_BUFFER_SIZE);
          check = 0;
        }
      }
    }
  }
  //continue register 
}

void loginC(int sd) {
  char buffer[MESSAGE_BUFFER_SIZE] = {0};
  int check = -1;
  
  while (check < 0) {
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    if (! strcmp(buffer, "username")) {
      if (check == -2) printf("That username does not exist\n");
      printf("username: ");
      fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
      buffer[strlen(buffer) - 1] = 0;
      write(sd, buffer, MESSAGE_BUFFER_SIZE);
      check = -2;
    }
    
    else if (! strcmp(buffer, "password")) {
      if (check == -3) printf("Incorrect password\n");
      printf("password: ");
      fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
      buffer[strlen(buffer) - 1] = 0;
      write(sd, buffer, MESSAGE_BUFFER_SIZE);
      check = -3;
    }
    
    else if (! strcmp(buffer, "successful login")) {
      check = 0;
    }
  }
}

void class_select(int sd) {
  char buffer[MESSAGE_BUFFER_SIZE] = {0};
  int check = 1;
  
  while (check) {
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    //printf("buffer: %s\n", buffer);
    if (! strcmp(buffer, "makeclass")) {
      printf("YES\n");
      while (check > 0) {
        printf("What is your preferred class?\n1. Warrior\n2. Mage\n3. Healer\nPick: ");
        fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = 0;
        if ( (! strcmp(buffer, "1")) || (! strcmp(buffer, "2")) || (! strcmp(buffer, "3"))) {
          write(sd, buffer, MESSAGE_BUFFER_SIZE);
          check = 0;
        }
      }
    }
    else if (! strcmp(buffer, "proceed")) {
      printf("Class is already decided\n");
      check = 0;
    }
  }
}
/*
void class_select(int sd, char buffer[]){
  int check = 1;
  
  while (check > 0){
    printf("I will fill your screen.\n");
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    if (! strcmp(buffer, "no class")) {
      printf("1.Warrior\n2.Mage\n3.Hunter\nChoose your class:");
      fgets(buffer,MESSAGE_BUFFER_SIZE,stdin);
      write(sd, buffer, MESSAGE_BUFFER_SIZE);
    }
    else if (! strcmp(buffer, "classy")){
      printf("Class chosen!\n");
      write(sd,"success",MESSAGE_BUFFER_SIZE);
      printf("end here\n");
      //return 0;
      check = 0;
    }
    printf("END HERE\n");
    //return 0;
  }
}
*/

void chatSYS(int sd, int * chat_set, char * curr_whisp) {
  char buffer[MESSAGE_BUFFER_SIZE];
  char buffer2[MESSAGE_BUFFER_SIZE];

  char * command;
  
  while (1) {
      if ( (*chat_set) == 0) printf(WHT);
      //else if ( (*chat_set) == 1) printf(GRN);
      //else if ( (*chat_set) == 2) printf(MAG);
      fgets(buffer, MESSAGE_BUFFER_SIZE, stdin);
      buffer[strlen(buffer) - 1] = 0;
      
      //printf("I'm here!\n");
      
      if (buffer[0] != '/') {
        if (*chat_set == 2) sprintf(buffer2, "%d %s %s", *chat_set, curr_whisp, buffer);
        else sprintf(buffer2, "%d %s", *chat_set, buffer);
        //printf("I'm here!\n");
        strcpy(buffer, buffer2);
        //printf("I'm here!\n");
        write(sd, buffer, MESSAGE_BUFFER_SIZE);
      }
      else {
        char * temp = (char *)malloc(MESSAGE_BUFFER_SIZE);
        //IS A COMMAND CHECK FOR CERTAIN COMMANDS
        printf("I'm in commandcheck!\n");
        printf("I'm here!\n");
        strcpy(temp, buffer);
        printf("I'm here!\n");
        strsep(&temp, "/");
        printf("I'm here!\n");
        command = strsep(&temp, " ");
        printf("I'm here!\n");
        printf("buffer: %s\n", buffer);
        //if ( (! strcmp(command, "lobby")) || (! strcmp(command, "l")) ) {
        //  *chat_set = 0;
        //}
        //else if ( (! strcmp(command, "party")) || (! strcmp(command, "p")) ) {
        //  *chat_set = 1;
        //}
        //else if ( (! strcmp(command, "whisper")) || (! strcmp(command, "w")) ) {
        //  *chat_set = 2;
        //  strcpy(curr_whisp, temp); 
        //}
        
        if (! strcmp(command, "help")) printf("%s\n", HELP);
        else write(sd, buffer, MESSAGE_BUFFER_SIZE);
        printf("CAN I COME HERE?\n");
      }
      //printf("I'm performing the write\n");
      //printf("sent message %s\n", buffer);
      //printf("I've done the write!!\n");
  }
}

void ENDCUR(int sd, int * chat_set, char * curr_whisp) { //does graphical display and ncurses 
  char buffer[MESSAGE_BUFFER_SIZE];
  char * buffer2 = (char *)malloc(MESSAGE_BUFFER_SIZE);
  char * lead;
  char * command;
  
  while (1) {
      read(sd, buffer, MESSAGE_BUFFER_SIZE);
      //printf("I've read something new!!!!\n");
      //printf("new message: %s\n", buffer);
      strcpy(buffer2, buffer);
      //printf("buffer2: %s\n", buffer2); 
      lead = strsep(&buffer2, " ");
      
      
      //printf("lead: %s\n", lead);
      //printf("buffer2: %s\n", buffer2);
      if (strcmp(lead, "/")) { //DO COLORS
        if (! strcmp(lead, "0")) printf(WHT "%s\n" RESET, buffer2);
        //else if (! strcmp(lead, "1")) printf(GRN "%s\n" RESET, buffer2);
        //else if (! strcmp(lead, "2")) printf(MAG "%s\n" RESET, buffer2);
        else if (! strcmp(lead, "3")) printf(CYN "%s\n" RESET, buffer2);
      }
      else {
        command = strsep(&buffer2, " ");
        if (! strcmp(command, "whisper")) {
          curr_whisp = strsep(&buffer2, " ");
          *chat_set = 2;
        }
      }
    }
  //else server command
}

int main( int argc, char *argv[] ) {
  char *host;
  if (argc != 2 ) {
    printf("host not specified, connecting to 127.0.0.1\n");
    host = "127.0.0.1";
  }
  else
    host = argv[1];

  int sd;

  sd = client_connect( host);
  char buffer[MESSAGE_BUFFER_SIZE];
  //char * input = buffer;
  
  int * chat_set = malloc(sizeof(int)); //chat_setting for players
  *chat_set = 0;
  char * curr_whisp = (char *)malloc(MESSAGE_BUFFER_SIZE);
  
  printf(CYN);
  
  int check = opening(sd); //asks whether they have an account
  if (check > 0) registerC(sd); //registers them
  else loginC(sd);  
  
  class_select(sd);
  
  printf("Welcome to the Game.\n" RESET);
  char buffer2[MESSAGE_BUFFER_SIZE];
  printf("Client: Made it past class_select\n");
  int subPID = fork();
  if (subPID) chatSYS(sd, chat_set, curr_whisp); //THIS SHOULD WORK RIGHT???
  else ENDCUR(sd, chat_set, curr_whisp);
  
  return 0;
}


/*
int pos = 0;
char Btoken, Atoken
Atoken = buffer[pos];
while (buffer[pos] != NULL) {
  Btoken = Atoken;
  Atoken = buffer[pos + 1];
  buffer[pos + 1] = Btoken;
  pos++;
}
buffer[0] = '/';
*/

