#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "networking.h"

void process( char * s );
void sub_server( int sd );



int checkUSER(char * name, int num) { 
  chdir("accounts/");
  int fd;
  if (num) fd = open(name, O_RDWR, 0);
  else fd = open(name, O_CREAT | O_EXCL, 0644);
  if (fd == -1) return 0;
  else return 1;
}

void registerr(int sd, character * player) {
  char name[MESSAGE_BUFFER_SIZE]; //name buffer
  char buffer[MESSAGE_BUFFER_SIZE];
  int check = 1; //while conditional
  
  while (check) { //checks conditional
    write(sd, "makeuser", 9); //sends message to client telling it to prompt
    read(sd, name, MESSAGE_BUFFER_SIZE); //read name the client sends
    if ( checkUSER(name, 0) ) { //checks availability
      printf("Username: %s is available\n", name);
      check = 0; //if available, end loop
      strcpy(player->cname, name); //requires cname to be char[]
    }
  }// else continue asking for name
  
  write(sd, "makepass", 9);
  read(sd, buffer, MESSAGE_BUFFER_SIZE);
  int fd = open(name, O_WRONLY, 0);
  write(fd, buffer, strlen(buffer)); //we want newline
  
}

int checkPASS(char * name, char * pass) {
  FILE * fp = fopen(name, "r");
  if (fp == NULL) return 0;
  else {
    char buffer[MESSAGE_BUFFER_SIZE];
    char token;
    char pos = 0;
    rewind(fp);
    while ( (token = getc(fp)) != '\n' && token != EOF) {
      buffer[pos++] = token;
    }
    buffer[pos] = 0;
    if (! strcmp(buffer, pass)) return 1;
    else return 0;
  }
}

void login(int sd, character * player) {
  char buffer[MESSAGE_BUFFER_SIZE];
  char name[MESSAGE_BUFFER_SIZE];
  int check = 1;
  
  printf("in username\n");
  
  while (check) {
    printf("writing username\n");
    write(sd, "username", 9);
    read(sd, name, MESSAGE_BUFFER_SIZE);
    printf("read username\n");
    if ( checkUSER(name, 1) ) {
      printf("username accepted\n");
      check = 0;
    }
  }
  strcpy(player->cname, name); //sets playername
  player->in_party = 0;
  
  check = 1;
  while (check) {
    write(sd, "password", 9);
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    if ( checkPASS(name, buffer) ) {
      printf("password accepted\n");
      check = 0;
    }
  }
  
  write(sd, "successful login", 17);
  
}

//THESE WILL HAVE TO BE USED CRUCIALLY FOR SAVE PROGRESS AND COMMUNICATION
char * convertS(character * player, char buffer[]) {
  printf("convertS used");
  sprintf(buffer, "%d %d %f %f %f %f %f %d %d %d %d %c",
  player->CLASS_ID,
  player->DUNGEON,   
  player->HP,
  player->ATK,
  player->MATK,
  player->DEF,
  player->MDEF,
  
  player->MOVE1_ID,
  player->MOVE2_ID,
  player->MOVE3_ID,
  player->MOVE4_ID,
  0);
  
  return buffer;
}
//converts player struct to a string we can write into accounts

void convertC(character *player, char *a) {
  char * info = strdup(a);
  //char * info;
  //strcpy(info, a);
  printf("convertC used\n");
  strsep(&info,"\n");
  if ( player->CLASS_ID = atoi(strsep(&info," "))) {
    printf("WE'RE INSIDE\n");
    player->DUNGEON = atoi(strsep(&info," "));
    printf("THIS ONE???\n");
    player->HP = atof(strsep(&info," "));
    printf("THIS ONE???\n");
    player->ATK = atof(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MATK = atof(strsep(&info," "));
    printf("THIS ONE???\n");
    player->DEF = atof(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MDEF = atof(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MOVE1_ID = atoi(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MOVE2_ID = atoi(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MOVE3_ID = atoi(strsep(&info," "));
    printf("THIS ONE???\n");
    player->MOVE4_ID = atoi(strsep(&info," "));
    printf("THIS ONE???\n");
  }
  printf("WE'RE OUT!\n");
  free(info);
} //converts string to player struct

void convertF(character * player) {
  printf("convertf start\n");
  int fd = open(player->cname, O_RDONLY);
  //printf("don't fuck me\n");
  char * a;
  //printf("don't fuck me\n");
  read(fd, a, MESSAGE_BUFFER_SIZE);
  //printf("don't fuck me\n");
  convertC(player, a);
  //printf("don't fuck me\n");
  printf("Player ClassID: %d\n", player->CLASS_ID);
  printf("COnvertF done\n");
}

void command(int sd, char buffer[], character * player) {
  //whisper ___ //party //lobby CLIENT
  //help CLIENT
  //use _ _
  char * commandS;
  char * name;
  printf("I actually got to command!!\n");
  
  strsep(&buffer, "/");
  commandS = strsep(&buffer, " ");
  if (! strcmp(commandS,"use")) {
    char * a = strsep(&buffer," ");
    char * b = strsep(&buffer," ");
    //attack(player, sd, a, b); //commented to try compiling
  }
  //else if ( (! strcmp(commandS, "reply")) || (! strcmp(commandS, "r"))) {
  //  if (strcmp(player->last_whisp, "")) {//exists
  //    sprintf(buffer, "/whisper %s", player->last_whisp);
//      write(sd, buffer, MESSAGE_BUFFER_SIZE);
//    }
  //  else write(sd, "3 [Server]: No one to reply to", 31);
  //} 
  if (! strcmp(commandS, "getkey")) {
    if (player->in_party) {
      char buffer2[MESSAGE_BUFFER_SIZE];
      printf("checking party\n");
      sprintf(buffer2, "3 [Server]: partykey is %d", player->party_key);
      printf("buffer2: %s\n", buffer2);
      write(sd, buffer2, MESSAGE_BUFFER_SIZE);
    }
    else write(sd, "3 [Server]: You are not in a party", 35);
  }
  else if (! strcmp(commandS, "createparty")) { //check in_party(or see if party_key) is 0, ftok create shared memory, set party_key
    //nt shmd = shmget(420, 1024, IPC_CREAT | 0664);
    //size of party 228 bytes
    if (! player->in_party) {
      player->in_party = 1;
      player->party_key = ftok(player->cname, getpid());
      int partyID = shmget(player->party_key, 228, IPC_CREAT | 0644);
      player->Party = shmat(partyID, 0, 0);
      
      strcpy(player->Party->leader.name, player->cname);
      player->Party->size = 1;
      player->Party->mate1.does_exist = 0;
      player->Party->mate2.does_exist = 0;
      player->Party->mate3.does_exist = 0;
      strcpy(player->Party->mate1.name, "lol");
      strcpy(player->Party->mate2.name, "lol");
      strcpy(player->Party->mate3.name, "lol");
      
      write(sd, "3 [SERVER]: Party has been successfully created", 48);
    }
    else write(sd, "3 [SERVER]: You are already in a party", 39);
  }
  else if (! strcmp(commandS, "joinparty")) { //join party, check party_key
  printf("I found joinparty!!\n");
    if (! player->in_party) {
      printf("I am not in a party!");
      player->party_key = (int) strtol(buffer, (char **)NULL, 10); //man this
      if ( (player->Party = shmat(shmget(player->party_key, 228, 0), 0, 0) ) != (void*) -1) { //error check //check this MAN
        if (player->Party->size < 4) {
          if (! player->Party->mate1.does_exist) {
            strcpy(player->Party->mate1.name, player->cname);
            player->Party->mate1.is_ready = 0;
            player->Party->mate1.does_exist = 1;
          }
          else if (! player->Party->mate2.does_exist) {
            strcpy(player->Party->mate2.name, player->cname);
            player->Party->mate2.is_ready = 0;
            player->Party->mate2.does_exist = 1;
          }
          else if (! player->Party->mate3.does_exist) {
            strcpy(player->Party->mate3.name, player->cname);
            player->Party->mate2.is_ready = 0;
            player->Party->mate2.does_exist = 1;
          }
          player->Party->size++;
          write(sd, "3 [Server]: Successfully joined party", 38);
        }
        else write(sd, "3 [Server]: Invalid partykey", 29);
      }
      else write(sd, "3 [Server]: Party is full :(", 29);
    }
    else write(sd, "3 [Server]: Already in a party", 31);
  }
  else if (! strcmp(commandS, "leaveparty")) {//if leader remove shared memory and initiate kicks, set party_key to 0
    if (player->in_party) {
      if (! strcmp(player->Party->leader.name, player->cname)) {
        //kick members
        if (player->Party->mate1.does_exist) {
          sprintf(name, "/kick %s", player->Party->mate1.name);
          command(sd, name, player);
        }
        if (player->Party->mate2.does_exist) {
          sprintf(name, "/kick %s", player->Party->mate2.name);
          command(sd, name, player);
        }
        if (player->Party->mate3.does_exist) {
          sprintf(name, "/kick %s", player->Party->mate3.name);
          command(sd, name, player);
        }
        //remove shared memory
        struct shmid_ds d;
        shmdt(player->Party);
        shmctl(player->party_key, IPC_RMID, &d); //look into this
        player->in_party = 0;
      }
      else {
        if (! strcmp(player->Party->mate1.name, player->cname)) {
          player->in_party = 0;
          player->Party->mate1.does_exist = 0;
          strcpy(player->Party->mate1.name, "lol");
          shmdt(player->Party);
        }
        else if (! strcmp(player->Party->mate2.name, player->cname)) {
          player->in_party = 0;
          player->Party->mate2.does_exist = 0;
          strcpy(player->Party->mate2.name, "lol");
          shmdt(player->Party);
        }
        else if (! strcmp(player->Party->mate3.name, player->cname)) {
          player->in_party = 0;
          player->Party->mate3.does_exist = 0;
          strcpy(player->Party->mate3.name, "lol");
          shmdt(player->Party);
        }
      }
    }
    else write(sd, "3 [Server]: You are not in a party", 35);
  }
  else if (! strcmp(commandS, "kick")) { //checks if in party and is leader, initiate kick sequence
    if (player->in_party) {
      if (! strcmp(player->Party->leader.name, player->cname)) {
        name = strsep(&buffer, " ");
        int shmd = shmget(420, 1024, 0);
        char * MEM = shmat(shmd, 0, 0);
        if ( (! strcmp(name, player->Party->mate1.name)) || (! strcmp(name, player->Party->mate2.name)) || (! strcmp(name, player->Party->mate3.name))) {
          sprintf(buffer, "/kicked %s ", name);
          strcpy(MEM, buffer);
        }
        else write(sd, "3 [Server]: Invalid player name", 32);
      }
      else write(sd, "3 [Server]: You must be the party leader", 41);
    }
    else write(sd, "3 [Server]: You must be the party leader", 41);
  }
  else if (! strcmp(commandS, "ready")) { //sets ready
    if (player->in_party) {
      if (! strcmp(player->Party->leader.name, player->cname)) write(sd, "3 [Server]: The partyleader cannot ready", 41);
      else {
        if (! strcmp(player->Party->mate1.name, player->cname)) {
          if (! player->Party->mate1.is_ready) {
            player->Party->mate1.is_ready = 1;
            write(sd, "3 [Server]: You have readied", 29);
          }
          else write(sd, "3 [Server]: You are already ready", 34);
        }
        else if (! strcmp(player->Party->mate2.name, player->cname)) {
          if (! player->Party->mate2.is_ready) {
            player->Party->mate2.is_ready = 1;
            write(sd, "3 [Server]: You have readied", 29);
          }
          else write(sd, "3 [Server]: You are already ready", 34);
        } 
        else if (! strcmp(player->Party->mate3.name, player->cname)) {
          if (! player->Party->mate3.is_ready) {
            player->Party->mate3.is_ready = 1;
            write(sd, "3 [Server]: You have readied", 29);
          }
          else write(sd, "3 [Server]: You are already ready", 34);
        } 
      }
    }
    else write(sd, "3 [Server]: You must be in a party to ready", 44);
  }
  else if (! strcmp(commandS, "start")) { //if leader and all ready, start
    //if (player->in_party && (! strcmp(player->Party->leader.name, player->cname) && (! (player->Party->mate1.does_exist && (! player->Party->mate1.is_ready))) && (! (player->Party->mate2.does_exist && (! player->Party->mate2.is_ready))) && (! (player->Party->mate3.does_exist && (! player->Party->mate.is_ready))))
    if (player->in_party) {
      if (! strcmp(player->Party->leader.name, player->cname)) {
        if ( (! (player->Party->mate1.does_exist && (! player->Party->mate1.is_ready))) && (! (player->Party->mate2.does_exist && (! player->Party->mate2.is_ready))) && (! (player->Party->mate3.does_exist && (! player->Party->mate3.is_ready))) ) {
          printf("This is where I enter the dungeon\n"); //dungeon difficulty
          //int diff;
          //startDungeon(sd, player->Party, diff);
          //startDungeon(sd,player->Party, diff);
          
        }
        else {
          if (player->Party->mate1.does_exist && (! player->Party->mate1.is_ready)) {
            sscanf(buffer, "3 [Server]: %s is not ready", player->Party->mate1.name);
            write(sd, buffer, MESSAGE_BUFFER_SIZE);
          }
          if (player->Party->mate1.does_exist && (! player->Party->mate2.is_ready)) {
            sscanf(buffer, "3 [Server]: %s is not ready", player->Party->mate2.name);
            write(sd, buffer, MESSAGE_BUFFER_SIZE);
          }
          if (player->Party->mate1.does_exist && (! player->Party->mate3.is_ready)) {
            sscanf(buffer, "3 [Server]: %s is not ready", player->Party->mate3.name);
            write(sd, buffer, MESSAGE_BUFFER_SIZE);
          }
        }
      }
      else write(sd, "3 [Server]: You're not the party leader", 40);
    }
    else write(sd, "3 [Server]: You're not in a party", 34);
  }
  else write(sd, "3 [Server]: Invalid Command", 29);
}

//void request(int sd, char buffer[]) 

void chat(int sd, char buffer[], character * player) {
  printf("I'm in chat!\n");
  //"char *strsep(char **stringp, const char *delim);"
  int shmd = shmget(420, 1024, 0);
  char * MEM = shmat(shmd, 0, 0);
  //char mode[MESSAGE_BUFFER_SIZE];
  char * mode;
  printf("received message: %s\n", buffer);
  //if (buffer[0] == '0') {
  printf("LOBBY CHAT!\n");
    //sscanf(buffer, "%d %s", mode, temp);
  strsep(&buffer, " ");
    //sprintf(MEM, "%s/%s/all/%s", mode, player->cname, buffer);  
  printf("I used sprintf to write to MEM!\n");
  sprintf(MEM, "0/%s/all/%s", player->cname, buffer);
  
  //else if (buffer[0] == '1') {
    //printf("PARTYCHAT!!!\n");
    //if (! player->in_party) write(sd, ) //DO IN CLIENT
    //sscanf(buffer, "%d %s", mode, temp);
    //CHECK IF PLAYER IS IN A PARTY!!!
    //mode = strsep(&buffer, " ");
    //printf("I'm here!!\n");
    //if (player->in_party) sprintf(MEM, "%s/%s/%s %s %s %s /%s", mode, player->cname, player->Party->leader.name, player->Party->mate1.name, player->Party->mate2.name, player->Party->mate3.name, buffer);  //kk
    //else write(sd, "3 [Server]: You are not in a party", 35);
  //}
  //else if (buffer[0] == '2') {
   // char * whispName;
    //sscanf(buffer, "%d %s %s", mode, whispName, temp);
    //set latest_whisp name!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   // mode = strsep(&buffer, " ");
   // whispName = strsep(&buffer, " ");
   // sprintf(MEM, "%s/%s/%s/%s", mode, player->cname, whispName, buffer);
  //}
  printf("I'm past the checks!\n");
  shmdt(MEM);
}


void interpret(int sd, character * player) {
  char buffer[MESSAGE_BUFFER_SIZE];
  printf("inside interpret!");
  while (1) {
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    printf("I received a message!\n");
    printf("message: %s\n", buffer);
    if (buffer[0] == '/') command(sd, buffer, player);
    else chat(sd, buffer, player);
  }
  
}

void update(int sd, character * player) {
  printf("I'm here in update!\n");
  int shmd = shmget(420, 1024, 0);
  printf("I'm here in update!\n");
  char * MEM = shmat(shmd, 0, 0); //struct message * m, address is just message m <-- pointer
  printf("I'm here in update!\n");
  char name[MESSAGE_BUFFER_SIZE] = {0};
  printf("I'm here in update!\n");
  char buffer[MESSAGE_BUFFER_SIZE];
  printf("I'm here in update!\n");
  char * temp;
  char * intended;
  char *author;
  char mode;
  
  strcpy(name, player->cname);
  name[strlen(name)] = ' ';
  printf("name: %s\n", name);
  printf("last_message: %s\n", player->last_message);
  
  while (1) {
    //printf("MEM: %s\n", MEM);
    //check shared memory
    if (strcmp(player->last_message, MEM) && (strstr(MEM, "all") != NULL || strstr(MEM, name) != NULL)) {
      printf("inside update and shared memory!\n");
      strcpy(player->last_message, MEM);
      printf("new last_message: %s\n", player->last_message);
      strcpy(temp, MEM);
      
      //sscanf(MEM, "%d/%s/%s/%s", mode, author, intended, message);
      mode = temp[0];
      printf("mode: %d", mode);
      if (mode == '/') {
        strsep(&temp, "/");
        author = strsep(&temp, "/");
        if (! strcmp(author, "kicked")) {
          player->in_party = 0;
          player->party_key = 0;
          shmdt(player->Party);
          write(sd, "3 [Server]: You have been removed from the party", 49);
        }
      }
      else {
        printf("In reading message in update!!\n");
        strsep(&temp, "/");
        author = strsep(&temp, "/");
        intended = strsep(&temp, "/");
        
        printf("mode: %d\n", mode);
        printf("author: %s\n", author);
        printf("intended: %s\n", intended);
        printf("temp: %s\n", temp);
        
        sprintf(buffer, "0 %s: %s", author, temp);
        //else if (mode == 49 && strstr(intended, name) != NULL) {
        //  
        //  sprintf(buffer, "%c/%s: %s", mode, author, temp);
        //  
        //}
        //else if (mode == 50 && ( (! strcmp(intended, player->cname)) || (! strcmp(author, player->cname)) ) ) {  //set last whispered
        //  if (! strcmp(author, player->cname)) sprintf(buffer, "%c To %s: %s", mode, intended, temp);
        //  else {
        //    sprintf(buffer, "%c From %s: %s", mode, author, temp);
        //    strcpy(player->last_whisp, author);
        //  }
        //}
        printf("message to write: %s\n", buffer);
        write(sd, buffer, MESSAGE_BUFFER_SIZE);
      }
    }
    
    
    //updateprintf("THIS ONE???\n");
    
  }
}

void save(character * player) {
  
}

void setup(int sd, character * player) {
  char buffer [MESSAGE_BUFFER_SIZE];
  //char * buffer = (char *)malloc(MESSAGE_BUFFER_SIZE);
  printf("1\n");
  convertF(player);
  printf("2\n");
  if (! player->CLASS_ID) {
    write(sd, "makeclass", 10);
    printf("3\n");
    read(sd, buffer, MESSAGE_BUFFER_SIZE);
    printf("4\n");
    if (! strcmp(buffer, "1")) convertC(player,"0\n1 1 500 100 100 100 100 1 1 1 1");
    else if (! strcmp(buffer, "2")) convertC(player,"0\n2 1 500 100 100 100 100 1 1 1 1");
    else if (! strcmp(buffer, "3")) convertC(player,"0\n3 1 500 100 100 100 100 1 1 1 1");
    printf("5\n");
    umask(0000);
    printf("6\n");
    int fd = open(player->cname,O_APPEND|O_WRONLY);
    printf("7\n");
    char * a = convertS(player,buffer);
    printf("8\n");
    char buffer[MESSAGE_BUFFER_SIZE];
    printf("9\n");
    strcpy(buffer,a);
    printf("10\n");
    write(fd, buffer ,MESSAGE_BUFFER_SIZE);
    printf("11\n");
    close(fd);
    printf("12\n");
  }
  else write(sd, "proceed", 8);
  printf("Server made it past setup.\n");
  //free(buffer);
}


//THIS SHIT DONT WORK :D PLEASE HELP ME :-:
/*
void setup(character * player, int sd) {
  printf("Server actually got to setup");
  char buffer[MESSAGE_BUFFER_SIZE];
  int check = 1;
  
  while (check > 0){
    printf("CLASS_ID: %d",player->CLASS_ID);
    if (player->CLASS_ID == 0){
      write(sd, "no class", MESSAGE_BUFFER_SIZE);
      read(sd, buffer, MESSAGE_BUFFER_SIZE);
      int classChoice = atoi(buffer); //atoi works?
      printf("classchoice: %d\n", classChoice);
      if (classChoice > 3 || classChoice <= 0 ) printf("Invalid Choice!\n");
      else if (classChoice == 1) convertC(player,"1 1 500 100 100 100 100 1 1 1 1");//Warrior
      else if (classChoice == 2) convertC(player,"2 1 500 100 100 100 100 1 1 1 1");//Mage
      else if (classChoice == 3) convertC(player,"3 1 500 100 100 100 100 1 1 1 1");//Hunter
    }
    else if (player->CLASS_ID != 0) {
      printf("CLASS_ID: %d",player->CLASS_ID);
      write(sd, "classy", MESSAGE_BUFFER_SIZE);
      read(sd, buffer, MESSAGE_BUFFER_SIZE);
      if (!strcmp(buffer, "success")){
        umask(0000);
        int fd = open(player->cname,O_APPEND|O_WRONLY);
        char * a = convertS(player,buffer);
        char buffer[MESSAGE_BUFFER_SIZE];
        strcpy(buffer,a);
        printf("yolo%s\nyolo\n",a);
        write(fd, buffer ,MESSAGE_BUFFER_SIZE);
        close(fd);
        printf("end1\n");
        //return 0;
        check = 0;
      }
      printf("end2\n");
      check = 0;
      //return 0
    }
  }
}
*/
void sub_server( int sd ) {
  character * player = malloc(sizeof(character));
  strcpy(player->cname, "lol");
  strcpy(player->last_whisp, "lol");
  strcpy(player->last_message,"lol");
  
  char buffer[MESSAGE_BUFFER_SIZE];
  
  read(sd, buffer, MESSAGE_BUFFER_SIZE);
  if (! strcmp(buffer,"yes")) login(sd, player); //does login procedure
  else registerr(sd, player);  
  
  setup(sd, player);
  
  printf("Server: Made it past setup\n");
  
  
  int pid = fork();
  if (pid) interpret(sd, player);
  else update(sd, player);
  
  free(player);
  
}



void process( char * s ) {

  while ( *s ) {
    *s = (*s - 'a' + 13) % 26 + 'a';
    s++;
  }
}


// char cname[MESSAGE_BUFFER_SIZE];
  /*
  read(sd, &player, sizeof(player));
  printf("%s connected\n",player->cname);

  char buffer[MESSAGE_BUFFER_SIZE];
  while (read( sd, buffer, sizeof(buffer) )) {
    printf("[SERVER %d] from %s received: %s\n", getpid(), player->cname, buffer );
    process( buffer );
    write( sd, buffer, sizeof(buffer));
  }
  */
  


int main() {

  int sd, connection;
  int count = 0;

  sd = server_setup();
  
  int shmd = shmget(420, 1024, IPC_CREAT | 0664);
  
  while (1) {

    connection = server_connect( sd );

    int f = fork();
    if ( f == 0 ) {

      close(sd);
      sub_server( connection );
      count +=1;
      exit(0);
    }
    else {
      close( connection );
    }
  }
  return 0;
}