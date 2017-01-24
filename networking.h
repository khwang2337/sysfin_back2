#ifndef NETWORKING_H
#define NETWORKING_H

#define MESSAGE_BUFFER_SIZE 200

#define HELP "" //FILL IN HELP

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct partymember {
  char does_exist;
  char name[MESSAGE_BUFFER_SIZE];
  struct character * player;
  int is_ready;
} partymember;

typedef struct party {
  partymember leader;
  partymember mate1;
  partymember mate2;
  partymember mate3;
  //dungeon curr_dungeon;
  int size;
} party;
/*
typedef struct dungeon {
  party p;
  int turn;
  character boss1;
  character boss2;
  character boss3;
} dungeon;
*/
typedef struct character {
  char cname[MESSAGE_BUFFER_SIZE];
  int CLASS_ID;
  int DUNGEON;
  
  
  float HP;
  float HP_LOST;
  float ATK;
  float MATK;
  float DEF;
  float MDEF;
  
  int MOVE1_ID;
  int MOVE2_ID;
  int MOVE3_ID;
  int MOVE4_ID;
  //Player only    what is this?
  int DC;
  //int current_chat;
  char last_whisp[MESSAGE_BUFFER_SIZE]; //IMPLEMENT IN UPDATE
  char last_message[MESSAGE_BUFFER_SIZE];
  int in_party; //may not need
  int party_key;
  party * Party;
  
  //Enemy only
  int CD;
  int CD_NOW;
  int ELEMENT;
  int MOVE5_ID;
  int MOVE6_ID;
  
  //For battle only
  int DEAD;
  
  int DOT;
  int DOT_TURN;
  int STUN;
  int STUN_TURN;
  
  float ATKBUFF;
  int ATKBUFF_TURNS;
  float MATKBUFF;
  int MATKBUFF_TURNS;
  float DEFBUFF;
  int DEFBUFF_TURNS;
  float MDEFBUFF;
  int MDEFBUFF_TURNS;
  
  float ATKDEB;
  int ATKDEB_TURNS;
  float MATKDEB;
  int MATKDEB_TURNS;
  float DEFDEB;
  int DEFDEB_TURNS;
  float MDEFDEB;
  int MDEFDEB_TURNS;
  
} character;

//struct charSTATS

/*
struct dungeon {
  charSTATS leadSTATS;
  charSTATS mate1STATS;
  charSTATS mate2STATS;
  charSTATS mate3STATS;
  int turn;
  enemy ENEMY;
}

struct enemy {
  STATS
}


*/





// #define PLAYER_SIZE (MESSAGE_BUFFER_SIZE+ (sizeof(int)*3))

void error_check( int i, char *s );

int server_setup();
int server_connect(int sd);

int client_connect( char *host );

#endif
