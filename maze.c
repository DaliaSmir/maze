#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "maze.h"

/*In this game we used: vertical and horizonal walls, portal, keys and doors.
Special additions:
1. Number of lives - the player have 3 lives at the beginning of the game.
                     if the player will lose all of his lives, the game will be over.
 
2. One sided door - [ the player can enter from right to left
                  - ] the player can enter from left to right
 
3. Bombs - There may be some bombs in a shape of find_tav(S_MEDUZA), they are invisible.
           if the player touches a bomb he will lose one of his lives and get a warning message.
 
4. Lighter - In order to protect our player from dying, we added a lighter which can light the bombs.
             how to use this feature: collect the lighter, in a shape of find_tav(S_LIGHTER). in any given time of the
             game, you can press on the 'L' key on your keyboard and see the bombs for one turn.

5. Hint - If you need a hint, press 'H' on your keyboard. The hint will appear above.

*/
 
int keys = 0;
int light = 0;
 
const char *msg_quit[] = {"Bye!", "Farewell!", "Goodbye!","Stupid"};
int size_quit = sizeof msg_quit / sizeof msg_quit[0];
 
const char *msg_start[] = {"Hey!\nPress A/S/D/W to move, Q to quit", "Welcome!\nPress A/S/D/W to move, Q to quit",
                           "Wuzaaap\nPress A/S/D/W to move, Q to quit","Shalom\nPress A/S/D/W to move, Q to quit"};
const char *msg_unknown[] = {"Unknown key pressed", "What are you doing?", "Wrong key pressed",
                             "Are you sure about this key?"};
const char *msg_ouch[] = {"Ouch!", "That's a wall!", "Maybe try another direction","You got stuck in the wall!"};
const char *msg_win[] = {"You win!", "You're the king of the world!", "You rock!","Cool, you win!"};
const char *msg_fun[] = {"Wheeee","Whoooo","It's a magical tile!","Coooool"};
const char *msg_keys[] = {"You don't have enough keys","Where is your key?","Go find the key",
                          "You need a key for this door"};
const char *msg_lives[] = {"Oops you are DEAD! the monster won","Only one live left!! Be carefull",
                           "Monster alert! Only two lives left!!"};
const char *msg_portal[] = {"PORTAL!!!!","whhouooho","whoa! Where am I now?"};
const char *msg_hint[] = {"Be carfull from hidden monsters","Collect the lighter","Just click Q and forfeit","Sometimes we need to sacrafice something in order to win"};
 
maze_t *create (unsigned maze_cols, unsigned maze_rows, location_t *p, location_t *t,const char *message)
{
    maze_t *res;
    //Allocate memory for the struct
    if (NULL == (res = (maze_t *)malloc(sizeof(maze_t)))) {
        printf("Error! Allocation Faild.\n");
        exit(1);
    }
 
    res->maze_cols = maze_cols;
    res->maze_rows = maze_rows;
   
    //current
    if (NULL == (res->current = malloc(sizeof(location_t)))) {
        free(res);
        printf("Error! Allocation Faild.\n");
        exit(1);
    }
    memcpy((void*)res->current, (const void*)p, sizeof(location_t));
 
    //target
    if (NULL == (res->target = malloc(sizeof(location_t)))) {
        free(res->current);
        free(res);
        printf("Error! Allocation Faild.\n");
        exit(1);
    }
    memcpy((void*)res->target, (const void*)t, sizeof(location_t));
   
    //matrix
    if (NULL == (res->mat = malloc(maze_rows * sizeof(char*)))) {
            free(res->current);
            free(res->target);
            free(res);
            printf("Error! Allocation Faild.\n");
            exit(1);
    }
    for (int i = 0; i < maze_rows; i++) {
        if (NULL == (res->mat[i] = malloc(maze_cols * sizeof(char*)))) {
            free(res->current);
            free(res->target);
            free(res->mat);
            free(res);
            printf("Error! Allocation Faild.\n");
            exit(1);
        }
    }
       
    if (NULL == (res->message = malloc(strlen(message) + 1))) {
        free(res->current);
        free(res->target);
        free(res->mat);
        free(res->message);
        free(res);
        printf("Error! Allocation Faild.\n");
        exit(1);
    }
    strcpy(res->message,message);
 
    return res;
 
}
 
 
int main(int argc, char **argv)
{
    char file_name[256];
    strcpy(file_name, argv[1]);
    maze_t *temp = NULL;
    temp = read_maze(file_name);
    print_maze(temp); // print play area
 
    set_terminal(true);
    bool game_over = false;
    do {
       game_over = handle_input(temp);
    } while (!game_over);
 
    set_terminal(false);
    return 0;
}
 
#define puts_no_newline(s) fputs(s, stdout)
 
void print_row(maze_t *player)
{
    for (int j = 0; j < player->maze_cols+2; j++) {
        puts_no_newline(S_WALL);
    }
    putchar('\n');
}
 
void print_tile(char *player, char str)
{
    char color[20];
    char *ptr = malloc(20);
    char str2[2];
    str2[0] = str;
    ptr = color;
    int i =0;
    while (*player != 'm') {
        color[i] = *player;
        i++;
        player++;
    }
    color[i] = *player;
    i+=1;
    color[i] = '\0';
    strcat(color,str2);
    strcat(color,COLOR_OFF);
    printf("%s", ptr);
 
}
 
char find_tav(char *player) //S_target '$fndjf'
{
    while (*player != 'm') {
        player++;
    }
    player++;
    char c;
    c = *player;
    return c;
}
 
 
maze_t *read_maze(char *input_file)
{
    FILE *input;  
    input = fopen(input_file,"r");
 
    if (input == NULL) {
        printf("File not valid!");
        exit(1);
    }
 
    char name[100] = "";
    char letter[2] ="" ;
    fgets(letter,2,(FILE*)input);
 
    while(letter[0] != '\n') {
        strcat(name,letter);
        fgets(letter,2,(FILE*)input);
    }
 
    int rows,columns ;
    fscanf((FILE*)input,"%d",&rows);
    fscanf((FILE*)input,"%d",&columns);
 
    location_t pl ;
    location_t tr ;
 
    maze_t *temp = NULL;
    temp = create(columns,rows,&pl,&tr,name);
    temp->lives = 3;
    temp->lighter = false;
   
    location_t *ptr_m;
    ptr_m = temp->meduza;
    location_t *ptr_p;
    ptr_p = temp->portals;
   
    fgets(letter,4,(FILE*)input);
    int valid_pl = -1;
    int valid_t = -1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns+1; j++) {
            fgets(letter,2,(FILE*)input);
 
            if (letter[0] == '\n') {
                continue;
            }
 
           if (find_tav(S_PLAYER) == letter[0]) {
                temp->mat[i][j] = '@';
                temp->current->i = i;
                temp->current->j = j;
                valid_pl++;
            } else if (find_tav(S_TARGET) == letter[0]) {
                temp->mat[i][j] = ' ';
                temp->target->i = i;
                temp->target->j = j;
                valid_t++;
            } else if (find_tav(S_MEDUZA) == letter[0]) {
                temp->mat[i][j] = letter[0];
                ptr_m->i = i;
                ptr_m->j = j;
                ptr_m++;
            } else if (find_tav(S_PORTAL) == letter[0]) {
                temp->mat[i][j] = letter[0];
                ptr_p->i = i;
                ptr_p->j = j;
                ptr_p++;
            } else if (find_tav(S_INNER_WALL) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_UPWALL) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_SIDEWALL) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_LEFT) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_RIGHT) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_KEY) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_LIGHTER) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (find_tav(S_DOOR) == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else if (' ' == letter[0]) {
                temp->mat[i][j] = letter[0];
            } else {
                printf("There are unknown tiles in this maze!");
                exit(1);
            }
        }
    }
 
    if (valid_pl != 0 || valid_t != 0) {
        printf("Invalid number of players or targets!");
        exit(1);
    }
 
    char c = ' ';
    temp->mat[temp->current->i][temp->current->j] = c;
    fclose(input);
       
return temp;
 
}
 
 
void print_maze(maze_t *player)
{
    static unsigned frame_num; // this variable retains its value between calls
    puts_no_newline(CLEAR_SCREEN); // clear terminal
    printf("%-*.*s%4d %s%d\n", player->maze_cols+25, player->maze_cols+25, player->message, frame_num++,"Lives:",player->lives); // frame number and message
    print_row(player); // print header row
    for (int i = 0; i < player->maze_rows; i++) { // print maze rows (including player)
        puts_no_newline(S_WALL);
        for (int j = 0; j < player->maze_cols; j++) {
            if ((player->target->i == i) && (player->target->j == j)) {
               if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_TARGET));
                } else {
                    puts_no_newline(S_TARGET);
                }
            } else if (player->mat[i][j] == find_tav(S_RIGHT)) {
                if ((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_RIGHT));
                } else {
                    puts_no_newline(S_RIGHT);
                }
            } else if(player->mat[i][j] == find_tav(S_LEFT)) {
                if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_LEFT));
                } else {
                    puts_no_newline(S_LEFT);
                }
            } else if(player->mat[i][j] == find_tav(S_DOOR)) {
                if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_DOOR));
                } else {
                    puts_no_newline(S_DOOR);
                }
            } else if(player->mat[i][j] == find_tav(S_PORTAL)) {
                if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_PORTAL));
                } else {
                    puts_no_newline(S_PORTAL);
                }
            } else if(player->mat[i][j] == find_tav(S_SIDEWALL)) {
                if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_SIDEWALL));
                } else {
                    puts_no_newline(S_SIDEWALL);
                }
            } else if(player->mat[i][j] == find_tav(S_UPWALL)) {
                if((player->current->i == i) && (player->current->j == j)) {
                    print_tile(S_PLAYER,find_tav(S_UPWALL));
                } else {
                    puts_no_newline(S_UPWALL);
                }
            } else if ((player->current->i == i) && (player->current->j == j)) {
               puts_no_newline(S_PLAYER);
            } else if((player->mat[i][j] == find_tav(S_INNER_WALL))) {
                puts_no_newline(S_INNER_WALL);
            } else if((player->mat[i][j] == find_tav(S_KEY))) {
                puts_no_newline(S_KEY);
            } else if((player->mat[i][j] == find_tav(S_LIGHTER))) {
                puts_no_newline(S_LIGHTER);
            } else if((player->mat[i][j] == find_tav(S_MEDUZA))) {
                if(player->lighter == true) {
                    print_tile(S_LIGHTER,find_tav(S_MEDUZA));
                } else {
                    puts_no_newline(S_MEDUZA);
                }
            } else {
                puts_no_newline(S_SPACE);
            }
        }
        puts_no_newline(S_WALL);
        putchar('\n');
    }
    print_row(player); // print footer row
 
}
 
 
bool handle_input(maze_t *player)
{
    const char *message = MSG_NORMAL;
    strcpy(player->message,message);
    player->lighter = false;
    int key = getchar(); // get user input
    switch(key) {
        case EOF:
        case 'q':
        case 'Q':   // exit game
            message = MSG_EXIT;
            strcpy(player->message,message);
            print_maze(player);
            return true;
            break;
        case 'a':
        case 'A':   // go left
        if ((player->current->j == 0) || (player->mat[player->current->i][player->current->j-1] == find_tav(S_INNER_WALL))
            || (player->mat[player->current->i][player->current->j-1] == find_tav(S_UPWALL))
            || (player->mat[player->current->i][player->current->j-1] == find_tav(S_LEFT))) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);
        } else if (player->current->i == player->target->i  && player->current->j-1 == player->target->j) {
            message = MSG_WIN;
            strcpy(player->message,message);
            player->current->j--;
            print_maze(player);
            return true;
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_SIDEWALL)) {
            message = MSG_NORMAL;
            strcpy(player->message,message);
            if (player->mat[player->current->i][player->current->j] ==  find_tav(S_SIDEWALL)) {
                player->current->j--;
            } else {
                player->current->j--;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_PORTAL)) {
            message = MSG_PORTAL;
            strcpy(player->message,message);
            location_t *ptr = player->portals;
            if (ptr->i == player->current->i && ptr->j == player->current->j-1) {
                ptr++;
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            } else {
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_RIGHT)) {
            message = MSG_FUN;
            strcpy(player->message,message);
            player->current->j--;
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_LIGHTER)) {
            player->mat[player->current->i][player->current->j-1] = ' ';
            light = 1;
            player->current->j--;
            print_maze(player);
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_DOOR)) {
            if (0 != keys) {
                message = MSG_FUN;
                strcpy(player->message,message);
                player->mat[player->current->i][player->current->j-1] = ' ';
                player->current->j--;
                keys--;
                print_maze(player);
            } else {
                message = MSG_KEYS;
                strcpy(player->message,message);
                print_maze(player);
            }
        } else if (player->mat[player->current->i][player->current->j] == find_tav(S_UPWALL)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j-1] == find_tav(S_KEY)) {
            keys++;
            player->mat[player->current->i][player->current->j-1] = ' ';
            player->current->j--;
            print_maze(player);
        } else if(player->mat[player->current->i][player->current->j-1] == find_tav(S_MEDUZA)) {
            player->lives--;
            strcpy(player->message,msg_lives[player->lives]);
            player->mat[player->current->i][player->current->j-1] = ' ';
            player->current->j--;
            print_maze(player);
 
            if (player->lives == 0) {
                strcpy(player->message,msg_lives[player->lives]);
                return true;
            }
        } else {
            player->current->j--;
            print_maze(player);
        }
        break;
        case 'd':
        case 'D':   // go right
        if ((player->current->j == player->maze_cols-1)
            || (player->mat[player->current->i][player->current->j+1] == find_tav(S_INNER_WALL))
            || (player->mat[player->current->i][player->current->j+1] == find_tav(S_UPWALL))
            || (player->mat[player->current->i][player->current->j+1] == find_tav(S_RIGHT))) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);
        } else if (player->current->i == player->target->i  && player->current->j+1 == player->target->j) {
            message = MSG_WIN;
            strcpy(player->message,message);
            player->current->j++;
            print_maze(player);
            return true;  
        } else if (player->mat[player->current->i][player->current->j+1] == find_tav(S_LEFT)) {
            message = MSG_FUN;
            strcpy(player->message,message);
            player->current->j++;
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j] == find_tav(S_UPWALL)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j+1] == find_tav(S_PORTAL)) {
            message = MSG_PORTAL;
            strcpy(player->message,message);
            location_t *ptr = player->portals;
 
            if(ptr->i == player->current->i && ptr->j == player->current->j+1) {
                ptr++;
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            } else {
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j+1] == find_tav(S_SIDEWALL)) {
            message = MSG_NORMAL;
            strcpy(player->message,message);
 
            if(player->mat[player->current->i][player->current->j] ==  find_tav(S_SIDEWALL)) {
                player->current->j++;
            } else {
                player->current->j++;
            }
            print_maze(player);  
        } else if(player->mat[player->current->i][player->current->j+1] == find_tav(S_LIGHTER)) {
            player->mat[player->current->i][player->current->j+1] = ' ';
            light = 1;
            player->current->j++;
            print_maze(player);
        } else if(player->mat[player->current->i][player->current->j+1] == find_tav(S_KEY)) {
            keys++;
            player->mat[player->current->i][player->current->j+1] = ' ';
            player->current->j++;
            print_maze(player);
        } else if(player->mat[player->current->i][player->current->j+1] == find_tav(S_MEDUZA)) {
            player->lives--;
            strcpy(player->message,msg_lives[player->lives]);
            player->mat[player->current->i][player->current->j+1] = ' ';
            player->current->j++;
            print_maze(player);
 
            if (player->lives == 0) {
                strcpy(player->message,msg_lives[player->lives]);
                return true;
            }
        } else if (player->mat[player->current->i][player->current->j+1] == find_tav(S_DOOR)) {
            if (keys != 0) {
                message = MSG_FUN;
                strcpy(player->message,message);
                player->mat[player->current->i][player->current->j+1] = ' ';
                player->current->j++;
                keys--;
                print_maze(player);
            } else {
                message = MSG_KEYS;
                strcpy(player->message,message);
                print_maze(player);
            }
        } else {
            player->current->j++;
            print_maze(player);
        }
        break;
 
        case 'w':
        case 'W':   // go up
        if ((player->current->i == 0) || (player->mat[player->current->i-1][player->current->j] == find_tav(S_INNER_WALL))
            || (player->mat[player->current->i-1][player->current->j] == find_tav(S_SIDEWALL))
            || (player->mat[player->current->i-1][player->current->j] == find_tav(S_RIGHT))
            || (player->mat[player->current->i-1][player->current->j] == find_tav(S_LEFT))) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if (player->current->i-1 == player->target->i  && player->current->j == player->target->j) {
            message = MSG_WIN;
            strcpy(player->message,message);
            player->current->i--;
            print_maze(player);
            return true;  
        } else if (player->mat[player->current->i-1][player->current->j] == find_tav(S_UPWALL)) {
            message = MSG_NORMAL;
            strcpy(player->message,message);
 
            if(player->mat[player->current->i][player->current->j] == find_tav(S_UPWALL)) {
                    player->current->i--;
            } else {
                player->current->i--;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i-1][player->current->j] == find_tav(S_DOOR)) {
            if (keys != 0) {
                message = MSG_FUN;
                strcpy(player->message,message);
                player->mat[player->current->i-1][player->current->j] = ' ';
                player->current->i--;
                keys--;
                print_maze(player);
            } else {
                message = MSG_KEYS;
                strcpy(player->message,message);
                print_maze(player);
            }
        } else if (player->mat[player->current->i-1][player->current->j] == find_tav(S_PORTAL)) {
            message = MSG_PORTAL;
            strcpy(player->message,message);
            location_t *ptr = player->portals;
 
            if(ptr->i == player->current->i-1 && ptr->j == player->current->j) {
                ptr++;
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            } else {
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j] == find_tav(S_SIDEWALL)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        }  else if (player->mat[player->current->i][player->current->j] == find_tav(S_RIGHT) ||
                    player->mat[player->current->i][player->current->j] == find_tav(S_LEFT)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if(player->mat[player->current->i-1][player->current->j] == find_tav(S_LIGHTER)) {
            light = 1;
            player->mat[player->current->i-1][player->current->j] = ' ';
            player->current->i--;
            print_maze(player);
        } else if(player->mat[player->current->i-1][player->current->j] == find_tav(S_KEY)) {
            keys++;
            player->mat[player->current->i-1][player->current->j] = ' ';
            player->current->i--;
            print_maze(player);
        } else if(player->mat[player->current->i-1][player->current->j] == find_tav(S_MEDUZA)) {
            player->lives--;
            strcpy(player->message,msg_lives[player->lives]);
            player->mat[player->current->i-1][player->current->j] = ' ';
            player->current->i--;
            print_maze(player);
 
            if (player->lives == 0) {
                strcpy(player->message,msg_lives[player->lives]);
                return true;
            }
        } else {
            player->current->i--;
            print_maze(player);
        }
        break;
 
        case 's':
        case 'S':   // go down
        if ( (player->current->i == player->maze_rows-1) ||
            (player->mat[player->current->i+1][player->current->j] == find_tav(S_INNER_WALL)) ||
            (player->mat[player->current->i+1][player->current->j] == find_tav(S_SIDEWALL)) ||
            (player->mat[player->current->i+1][player->current->j] == find_tav(S_RIGHT)) ||
            (player->mat[player->current->i+1][player->current->j] == find_tav(S_LEFT))) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if ( player->current->i+1 == player->target->i  && player->current->j == player->target->j) {
            message = MSG_WIN;
            strcpy(player->message,message);
            player->current->i++;
            print_maze(player);
            return true;    
        } else if (player->mat[player->current->i+1][player->current->j] == find_tav(S_PORTAL)) {
            message = MSG_PORTAL;
            strcpy(player->message,message);
            location_t *ptr = player->portals;
 
            if(ptr->i == player->current->i+1 && ptr->j == player->current->j) {
                ptr++;
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            } else {
                player->current->i = ptr->i;
                player->current->j = ptr->j;
            }
            print_maze(player);  
        } else if (player->mat[player->current->i+1][player->current->j] == find_tav(S_UPWALL)) {
            message = MSG_NORMAL;
            strcpy(player->message,message);
 
            if(player->mat[player->current->i][player->current->j] == find_tav(S_UPWALL)) {
                    player->current->i++;
            } else {
            player->current->i++;
            }
            print_maze(player);
 
        } else if (player->mat[player->current->i+1][player->current->j] == find_tav(S_DOOR)) {
            if (keys != 0) {
                message = MSG_FUN;
                strcpy(player->message,message);
                player->mat[player->current->i+1][player->current->j] = ' ';
                player->current->i++;
                keys--;
                print_maze(player);
            } else {
                message = MSG_KEYS;
                strcpy(player->message,message);
                print_maze(player);
            }
        } else if (player->mat[player->current->i][player->current->j] == find_tav(S_SIDEWALL)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if (player->mat[player->current->i][player->current->j] == find_tav(S_RIGHT) ||
                    player->mat[player->current->i][player->current->j] == find_tav(S_LEFT)) {
            message = MSG_OUCH;
            strcpy(player->message,message);
            print_maze(player);  
        } else if(player->mat[player->current->i+1][player->current->j] == find_tav(S_KEY)) {
            keys++;
            player->mat[player->current->i+1][player->current->j] = ' ';
            player->current->i++;
            print_maze(player);
        } else if(player->mat[player->current->i+1][player->current->j] == find_tav(S_LIGHTER)) {
            light = 1;
            player->mat[player->current->i+1][player->current->j] = ' ';
            player->current->i++;
            print_maze(player);
        } else if(player->mat[player->current->i+1][player->current->j] == find_tav(S_MEDUZA)) {
            player->lives--;
            strcpy(player->message,msg_lives[player->lives]);
            player->mat[player->current->i+1][player->current->j] = ' ';
            player->current->i++;
            print_maze(player);
            if (player->lives == 0) {
                strcpy(player->message,msg_lives[player->lives]);
                return true;
            }
        } else {
            player->current->i++;
            print_maze(player);
        }
        break;
 
        case 'L':
        case 'l':
        if (0 == light) {
            strcpy(player->message,"No lighter!");
            print_maze(player);
        } else {
            player->lighter = true;
            light-=1;
            print_maze(player);
        }
        break;
        case 'H':
        case 'h':
        message = MSG_HINT;
        strcpy(player->message,message);
        print_maze(player);
        break;
        default:
            message = MSG_UNKNOWN;
            strcpy(player->message,message);
            print_maze(player);
    }
 
    return false;
}