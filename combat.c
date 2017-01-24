#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>

#define MESSAGE_BUFFER_SIZE 50

/*typedef struct partymember {
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

typedef struct character {
  char cname[MESSAGE_BUFFER_SIZE];
  int CLASS_ID;
  int DUNGEON;
  
  int HP;
  int HP_LOST;
  int ATK;
  int MATK;
  int DEF;
  int MDEF;
  
  int MOVE1_ID;
  int MOVE2_ID;
  int MOVE3_ID;
  int MOVE4_ID;
  //Player only
  int DC;
  //int current_chat;
  char latest_cname[MESSAGE_BUFFER_SIZE]; //IMPLEMENT IN UPDATE
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
  
  int ATKBUFF;
  int ATKBUFF_TURNS;
  int MATKBUFF;
  int MATKBUFF_TURNS;
  int DEFBUFF;
  int DEFBUFF_TURNS;
  int MDEFBUFF;
  int MDEFBUFF_TURNS;
  
  int ATKDEB;
  int ATKDEB_TURNS;
  int MATKDEB;
  int MATKDEB_TURNS;
  int DEFDEB;
  int DEFDEB_TURNS;
  int MDEFDEB;
  int MDEFDEB_TURNS;
  
} character;

/*
IN FILE:
password
cname
player->CLASS_ID
player->DUNGEON   
player->HP
player->ATK
player->MATK
player->DEF
player->MDEF
player->MOVE1_ID
player->MOVE2_ID
player->MOVE3_ID
player->MOVE4_ID
*/
int endDungeon(int a) {
    return !a;
}
//LOL had this in server already btw
void convertC(character *player, char *info) {
  printf("convertC used");
  char *fixInfo = strdup(info);
  player->CLASS_ID = atoi(strsep(&info," "));
  player->DUNGEON = atoi(strsep(&info," "));
  player->HP = atoi(strsep(&info," "));
  player->ATK = atoi(strsep(&info," "));
  player->MATK = atoi(strsep(&info," "));
  player->DEF = atoi(strsep(&info," "));
  player->MDEF = atoi(strsep(&info," "));
  player->MOVE1_ID = atoi(strsep(&info," "));
  player->MOVE2_ID = atoi(strsep(&info," "));
  player->MOVE3_ID = atoi(strsep(&info," "));
  player->MOVE4_ID = atoi(strsep(&info," "));
  free(fixInfo);
} //converts string to player struct


/*
void processStats(character a, char stats) {
    if(counter ==1) {
        a.cname = stats;
    }
    if(counter == 2) {
        a.HP = atoi(stats);
    }
    if(counter ==3) {
        a.ATK = atoi(stats);
    }
    if(counter ==4) {
        a.MATK = atoi(stats);
    }
    if(counter ==5) {
        a.DEF = atoi(stats);
    }
    if(counter ==6) {
        a.MOVE1_ID = atoi(stats);
    }
    if(counter ==7) {
        a.MOVE2_ID = atoi(stats);
    }
    if(counter ==8) {
        a.MOVE3_ID = atoi(stats);
    }
    if(counter ==9) {
        a.MOVE4_ID = atoi(stats);
    }
}
*/

int isDead(character* a) {
    if(a->HP ==0) {
        return 1;
    }
    else{
        return 0;
    }
}
int formula(int atk, int atkbuff,int atkdeb,int multipler,int def,int defbuff,int defdeb) {
    return ((atk * atk * (1-atkdeb) * (1+atkbuff)*multipler)/(def * defbuff * defdeb));
}

int heal(int hp_lost,int heal) {
    if (hp_lost < heal) {
        return 0;
    }
    return (hp_lost - heal);
}

int attack( character* player,character* target,int move) {
//	check player's class;
//	decrease all the timers on debuff and buffs for both player and target(if a timer reaches zero, turn buff/debuff back to 0)
//	decrease timers on stun and poison

    int cID = player->CLASS_ID;
    //CLASS: Warrior
	if (cID == 1) {
		if (move == 1) {                     //Skill Name: Swing			
		    if (player->MOVE1_ID == 1) {	 //Description: physical attack
				target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,1,target->DEF,target->DEFBUFF,target->DEFDEB);
			}
			if (player->MOVE1_ID == 2) {	 //Description: 1.2 physical attack with defense debuff 
				target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,1.2,target->DEF,target->DEFBUFF,target->DEFDEB);
				target->DEFDEB = 0.2;
				target->DEFDEB_TURNS = 3;
			}
			if (player->MOVE1_ID == 3) {	 //Description: 1.2 physical attack with attack buff
				target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,1.2,target->DEF,target->DEFBUFF,target->DEFDEB);
				player->ATKBUFF = 0.2;
				player->ATKBUFF_TURNS = 1;
			}
		}
	    if (move == 2) {                     //Skill Name: Armor up
	        if (player->MOVE2_ID == 1) {     //Description: Raises Def Buff by 1.3 times
	            player->DEFBUFF = 0.3;
	            player->DEFBUFF_TURNS = 3;
	        }
	        if (player->MOVE2_ID == 2) {     //Description: Magic Def up by  1.5
	            player->MDEFBUFF = 0.5;
	            player->MDEFBUFF_TURNS = 3;
	        }
	        if (player->MOVE2_ID == 3) {     //Description: Phys and Mag Def up by 1.3
	            player->DEFBUFF = 0.3;
	            player->DEFBUFF_TURNS = 3;
	            player->MDEFBUFF = 0.3;
	            player->MDEFBUFF_TURNS = 3;
	        }
	    }
	    if (move == 3) {                     //Skill Name: Armor break
	        if (player->MOVE3_ID == 1) {     //Description: Deals .8 damage + lowers enemy DEF by 20
	            target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,.8,target->DEF,target->DEFBUFF,target->DEFDEB);
	            target->DEFDEB = 0.2;
	            target->DEFDEB_TURNS = 2;
	        }
	        if (player->MOVE3_ID == 2) {     //Description: Deals .75 damage + stuns enemy for 2 turn 
	            target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,.75,target->DEF,target->DEFBUFF,target->DEFDEB);
	            target->STUN = 1;
	            target->STUN_TURN = 2;
	        }
	        if (player->MOVE3_ID == 3) {     //Description: Deal .1 damage + destorys the enemy def and sets it to half for 3 turns.
	            target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,.1,target->DEF,target->DEFBUFF,target->DEFDEB);
	            target->DEFDEB = 1;
	            target->DEFDEB_TURNS = 3;
	        }
	    }
	    if (move == 4) {                     //Skill Name: Final Stand
	        if (player->MOVE4_ID == 1) {     //Description:Deal tons of damage to enemy(5) but reduce own hp by .25
	            target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,5,target->DEF,target->DEFBUFF,target->DEFDEB);
	            player->HP_LOST -= ((player->HP)/4);
	        }
	        if (player->MOVE4_ID == 2) {     //Description:Raise own def by 10x, lowers atk by 5x
	            player->DEFBUFF = 10;
	            player->DEFBUFF_TURNS = 3;
	            player->ATKDEB = .8;
	            player->ATKDEB_TURNS = 3;
	        }
	        if (player->MOVE4_ID == 3) {     //Description:Deals fatal damage(10) but reduce hp by .50
	            target->HP_LOST += formula(player->ATK,player->ATKBUFF,player->ATKDEB,10,target->DEF,target->DEFBUFF,target->DEFDEB);
	            player->HP_LOST -= ((player->HP)/2);
	        }
	    }
	}
	
	//CLASS: Mage
	if (cID == 2) {
	    if (move == 1) {                     //Skill Name: Fire ball
	        if (player->MOVE1_ID == 1) {     //Description:Deal 1.0 mag atk
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,1,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	        }
	        if (player->MOVE1_ID == 2) {     //Description:Deal 1.0 mag atk with mag buff(1.3x) for 2 turns
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,1.3,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            player->MATKBUFF = .3;
	            player->MATKBUFF_TURNS;
	        }
	        if (player->MOVE1_ID == 3) {     //Description:Deal 1.1 mag atk and stuns enemy for 1 turn
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,1.1,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            target->STUN = 1;
	            target->STUN_TURN = 1;
	        }
	    }
	    if (move == 2) {                     //Skill Name: Poison blast
	        if (player->MOVE2_ID == 1) {     //Description:Does no dmg but applies a DOT (.3x) for 6 turns
	            target->DOT = .3;
	            target->DOT_TURN = 6;
	        }
	        if (player->MOVE2_ID == 2) {     //Description:Deals 1.2 initial damage and applies DOT(.6) for 3 turns(ignores def)
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,1.2,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            target->DOT = .6;
	            target->DOT_TURN = 3;
	        }
	        if (player->MOVE2_ID == 3) {     //Description:Deals 1x mag atk over 4 turns
	            target->DOT = 1;
	            target->DOT_TURN = 4;
	        }
	    }
	    if (move == 3) {                     //Skill Name: Ice storm
	        if (player->MOVE3_ID == 1) {     //Description: stuns enemy for 2 turns
	            target->STUN = 1;
	            target->STUN_TURN = 2;
	        }
	        if (player->MOVE3_ID == 2) {     //Description: Deals 1.5x damage if enemy stunned cced, do double damage(3x)
	            if(target->STUN){
	            	target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,3,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            }
	            else{
	                target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,1.5,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            }
	            
	        }
	        if (player->MOVE3_ID == 3) {     //Description: deals 4x damage over 2 turns and stun for 1 turn
	            target->DOT = 2;
	            target->DOT_TURN =2;
	            target->STUN = 1;
	            target->STUN_TURN = 1;
	            
	        }
	    }
	    if (move == 4) {                     //Skill Name: Meteor
	        if (player->MOVE4_ID == 1) {     //Description: 2.5 Magic atk + self stun 1 turn
	        	target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,2.5,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	        	player->STUN = 1;
	        	player->STUN_TURN =1;
	        }
	        if (player->MOVE4_ID == 2) {     //Description: 6 magic atk + self stun 2 turn
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,6,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            player->STUN = 1;
	            player->STUN_TURN = 2;
	        }
	        if (player->MOVE4_ID == 3) {     //Description: 3 magic atk 
	            target->HP_LOST += formula(player->MATK,player->MATKBUFF,player->MATKDEB,3,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	        }
	    }
	}
	
	//CLASS: Healer
	if (cID == 3) {
	    if (move == 1) {                     //Skill Name: Cleanse
	        if (player->MOVE1_ID == 1) {     //Description: removes all statuses
	            target->ATKDEB == 0;
	            target->ATKDEB_TURNS == 0;
	            target->MATKDEB == 0;
	            target->MATKDEB_TURNS == 0;
	            target->DEFDEB == 0;
	            target->DEFDEB_TURNS == 0;
	            target->MDEFDEB == 0;
	            target->MDEFDEB_TURNS == 0;
	        }
	        if (player->MOVE1_ID == 2) {     //Description: remove all statuses + heal
	            target->ATKDEB == 0;
	            target->ATKDEB_TURNS == 0;
	            target->MATKDEB == 0;
	            target->MATKDEB_TURNS == 0;
	            target->DEFDEB == 0;
	            target->DEFDEB_TURNS == 0;
	            target->MDEFDEB == 0;
	            target->MDEFDEB_TURNS == 0;
	            
	            target->HP_LOST = heal(target->HP_LOST,1000);
	        }
	        if (player->MOVE1_ID == 3) {     //Description: remove all statuses + buff atk/matk
	            target->ATKDEB == 0;
	            target->ATKDEB_TURNS == 0;
	            target->MATKDEB == 0;
	            target->MATKDEB_TURNS == 0;
	            target->DEFDEB == 0;
	            target->DEFDEB_TURNS == 0;
	            target->MDEFDEB == 0;
	            target->MDEFDEB_TURNS == 0;
	            
	            target->ATKBUFF == 20;
	            target->ATKBUFF_TURNS == 3;
	            target->MATKBUFF == 20;
	            target->MATKBUFF_TURNS == 3;
	        }
	    }
	    if (move == 2) {                     //Skill Name: Heal
	        if (player->MOVE2_ID == 1) {     //Description: Heal 1000 health
	            target->HP_LOST = heal(target->HP_LOST,1000);
	        }
	        if (player->MOVE2_ID == 2) {     //Description: Heal party 1000 health
	            target->HP_LOST = heal(target->HP_LOST,1000);
	        }
	        if (player->MOVE2_ID == 3) {     //Description: Heal 3000 health
	            target->HP_LOST = heal(target->HP_LOST,3000);
	        }
	    }
	    if (move == 3) {                     //Skill Name: Divine Protection
	        if (player->MOVE3_ID == 1) {     //Description: AOE 1.2 MDEF buff
	            target->MDEFBUFF = .2;
	            target->MDEFBUFF_TURNS = 3;
	        }
	        if (player->MOVE3_ID == 2) {     //Description: AOE 1.2 MDEF/DEF buff
	            target->MDEFBUFF = .2;
	            target->MDEFBUFF_TURNS = 3;
	            target->DEFBUFF = .2;
	            target->DEFBUFF_TURNS = 3;
	        }
	        if (player->MOVE3_ID == 3) {     //Description: AOE 1.2 MDEF/MATK buff
	            target->MDEFBUFF = .2;
	            target->MDEFBUFF_TURNS = 3;
	            target->MATKBUFF = .2;
	            target->MATKBUFF_TURNS = 3;
	        }
	    }
	    if (move == 4) {                     //Skill Name: Holy Strike
	        if (player->MOVE4_ID == 1) {     //Description: 1x using mdef + self heal 500
	            target->HP_LOST += formula(player->MDEF,player->MDEFBUFF,player->MDEFDEB,1,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            player->HP_LOST = heal(player->HP_LOST,500);
	        }
	        if (player->MOVE4_ID == 2) {     //Description: 1x using mdef + self heal 500 + self buff 1.5 MDEF/DEF
	            target->HP_LOST += formula(player->MDEF,player->MDEFBUFF,player->MDEFDEB,1,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            player->HP_LOST = heal(player->HP_LOST,500);
	            player->MDEFBUFF = .5;
	            player->MDEFBUFF_TURNS = 3;
	            player->DEFBUFF = .5;
	            player->DEFBUFF_TURNS = 3;
	        }
	        if (player->MOVE4_ID == 3) {     //Description: 2x using mdef + self heal 2000
	            target->HP_LOST += formula(player->MDEF,player->MDEFBUFF,player->MDEFDEB,2,target->MDEF,target->MDEFBUFF,target->MDEFDEB);
	            player->HP_LOST = heal(player->HP_LOST,200);
	        }
	    }
	}
	
	//CLASS: Assassin
/*	if (cID == 4) {
	    if (move == 1) {                     //Skill Name: 
	        if (player->MOVE1_ID == 1) {     //Description:
	            
	        }
	        if (player->MOVE1_ID == 2) {     //Description:
	            
	        }
	        if (player->MOVE1_ID == 3) {     //Description:
	            
	        }
	    }
	    if (move == 2) {                     //Skill Name: 
	        if (player->MOVE2_ID == 1) {     //Description:
	            
	        }
	        if (player->MOVE2_ID == 2) {     //Description:
	            
	        }
	        if (player->MOVE2_ID == 3) {     //Description:
	            
	        }
	    }
	    if (move == 3) {                     //Skill Name: 
	        if (player->MOVE3_ID == 1) {     //Description:
	            
	        }
	        if (player->MOVE3_ID == 2) {     //Description:
	            
	        }
	        if (player->MOVE3_ID == 3) {     //Description:
	            
	        }
	    }
	    if (move == 4) {                     //Skill Name: 
	        if (player->MOVE4_ID == 1) {     //Description:
	            
	        }
	        if (player->MOVE4_ID == 2) {     //Description:
	            
	        }
	        if (player->MOVE4_ID == 3) {     //Description:
	            
	        }
	    }
	}*/
}

//int check();
    
void action(character *player, int sd, int a, int b) {
    int move;
    if (a == 1) move = player->MOVE1_ID;
    if (a == 2) move = player->MOVE2_ID;
    if (a == 3) move = player->MOVE3_ID;
    if (a == 4) move = player->MOVE4_ID;
    int type = check(player->CLASS_ID,move);
    
    //single + buff
    if (type == 1) {
        
    }
}

//literally only sets everything up
/*void startBattle(party * p,character *enemy, character *attacker, int move, int target) {
    srand(time(NULL));
    int deathCount = 0;
    int inProgress = 1;
    //int k;
    while(!isDead(enemy)){
    if (enemy->CD_NOW == 0) {
        int r = rand()%5;
        //determine (enemy, party, enemy, r);
        enemy->CD_NOW = enemy->CD;
    }
        
    //Checks for death of party
    /*for(k;k<sizeof(party);k++) {
            if(isDead(party->)) deathCount ++;
        }*/
/*       if(deathCount == sizeof(party)) {
            printf("All your party members are dead!\n");
        }
        deathCount = 0;
        
        //Checks for death of enemy
        if (isDead(enemy)) {
            printf("Enemy is eliminated!\n");
        }
    }
} */
/*void levelup(character * player) {
    
} */

int startBattle( party * p, character * enemy){
	printf("Battle starting against %s!\n", enemy->cname);
	shmrd = shmget(500,MESSAGE_BUFFER_SIZE,IPC_CREAT|0644);
	shmwr = shmget(501,MESSAGE_BUFFER_SIZE,IPC_CREAT|0644);
	// int count = 0;
	while (1){
		if (p->leader->HP>0){
			printf("%s's Turn!\n", p->leader->name);
			char ** cmess = shmat(shmwr,0,0);
			strcpy(cmess, "leader's turn!:\n");
			// read and attack
			//print out moves?
			int * choice = shmat(shmrd,0,0);
			attack(p->leader->player,enemy,choice);
			shmdt(cmess);
			shmdt(choice);
		}
		
		if(enemy->HP==0){
			//end battle
		}
		
		if (p->mate1->HP>0){
			printf("%s's Turn!\n", p->mate1->name);
			char ** cmess = shmat(shmwr,0,0);
			strcpy(cmess, "mate1's turn!:\n");
			// read and attack
			
			int * choice = shmat(shmrd,0,0);
			attack(p->mate1->player,enemy,choice);
			shmdt(cmess);
			shmdt(choice);
		}
		
		if(enemy->HP==0){
			//end battle
		}
		
		if (p->mate2->HP>0){
			printf("%s's Turn!\n", p->mate2->name);
			char ** cmess = shmat(shmwr,0,0);
			strcpy(cmess, "mate2's turn!:\n");
			// read and attack
			
			int * choice = shmat(shmrd,0,0);
			attack(p->mate2->player,enemy,choice);
			shmdt(cmess);
			shmdt(choice);
		}
		
		if(enemy->HP==0){
			//end battle
		}
		
		if (p->mate3->HP>0){
			printf("%s's Turn!\n", p->mate3->name);
			char ** cmess = shmat(shmwr,0,0);
			strcpy(cmess, "mate3's turn!:\n");
			// read and attack
			
			int * choice = shmat(shmrd,0,0);
			attack(p->mate3->player,enemy,choice);
			shmdt(cmess);
			shmdt(choice);
		}
		
		if(enemy->HP==0){
			//end battle, players win
			return 1;
		}
		
		printf("%s's Turn!\n", enemy->cname);
		char ** cmess = shmat(shmwr,0,0);
		strcpy(cmess, "enemy's turn!:\n");
		// read and attack
		attack(enemy,mate1,1);
		shmdt(cmess);
		shmdt(choice);
		
		if((p->leader->HP==0)&&(p->mate1->HP==0)&&(p->mate2->HP==0)&&(p->mate3->HP==0)){
			//end battle, players lose
			return 0;
		}
	}
}



void startDungeon(int sd, party * p, int dungeonDifficulty) {
    printf("You have entered %d\n", dungeonDifficulty);//welcome msg, subject to change
    chdir("dungeons");
    char *dungeonName;
    char *enemyStat;
    character *enemy;
    
    if(dungeonDifficulty ==1) {
        strcpy(dungeonName,"dungeon1.txt");
    }
    else if(dungeonDifficulty ==2) {
        strcpy(dungeonName,"dungeon2.txt");
    }
    else{
        strcpy(dungeonName,"dungeon3.txt");
    }
        
    FILE *fd = fopen(dungeonName, "r");
    int counter = 0;
    if (fd != NULL) {
        char *line;
        while(fgets (line, sizeof(line),fd) != NULL) {
            
            counter++;
            char * first = strsep(&line, " ");
            
            if(!strcmp("Floor",first)) {
                printf("Floor %s", line);
            }
            
            else {
                processStats(*enemy,line);
                printf("You have encountered %s!\n",enemy->cname);
                if(!startBattle(p, enemy)){
                	//players lost, restart?
                }
                else{
                	//players won, next battle
                }
            }
        }
        printf("You have conquered the dungeon!\n\n Returning to lobby.....");
    }
    //code to disband everyone from party
}

