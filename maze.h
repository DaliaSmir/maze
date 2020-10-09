#include <stdbool.h>
#define MAX_SIZE 1000

typedef struct {
    unsigned i;
    unsigned j;
} location_t;


typedef struct {
    location_t *current;
    location_t *target;
    location_t meduza[50];
    location_t portals[2];
    unsigned lives;
    bool lighter;
    unsigned maze_cols;
    unsigned maze_rows;
    char *message;
    char **mat;
} maze_t ;

#define SIZE_MSG(arr) (sizeof(arr)/sizeof(arr[0]))

#define MSG_NORMAL  "Press A/S/D/W to move, Q to quit"
#define MSG_START   msg_start[rand() % SIZE_MSG(msg_start)] 
#define MSG_UNKNOWN msg_unknown[rand() % SIZE_MSG(msg_unknown)]
#define MSG_EXIT msg_quit[rand() % SIZE_MSG(msg_quit)]
#define MSG_OUCH msg_ouch[rand() % SIZE_MSG(msg_ouch)]
#define MSG_WIN msg_win[rand() % SIZE_MSG(msg_win)]
#define MSG_FUN msg_fun[rand() % SIZE_MSG(msg_fun)]
#define MSG_KEYS msg_keys[rand() % SIZE_MSG(msg_keys)]
#define MSG_PORTAL msg_portal[rand() % SIZE_MSG(msg_portal)]
#define MSG_HINT msg_hint[rand() % SIZE_MSG(msg_hint) ]

maze_t *create(unsigned maze_cols, unsigned maze_rows, location_t *p, location_t *t,const char *message);
void set_terminal(bool start);
void print_tile(char *player, char str);
maze_t *read_maze(char *input_file);
void print_table(char **table, int rows, int cols);
void print_maze(maze_t *player);
bool handle_input(maze_t *player);
char find_tav(char *player);


/************************************
 * TERMINAL CODES ("MAGIC STRINGS") *
 ************************************/

#define COLOR_OFF       "\e[0m"
#define COLOR_BLACK     "\e[0;30m"
#define COLOR_GRAY      "\e[1;30m"
#define COLOR_RED       "\e[0;31m"
#define COLOR_LT_RED    "\e[1;31m"
#define COLOR_GREEN     "\e[0;32m"
#define COLOR_LT_GREEN  "\e[1;32m"
#define COLOR_BROWN     "\e[0;33m"
#define COLOR_YELLOW    "\e[1;33m"
#define COLOR_BLUE      "\e[0;34m"
#define COLOR_LT_BLUE   "\e[1;34m"
#define COLOR_PURPLE    "\e[0;35m"
#define COLOR_LT_PURPLE "\e[1;35m"
#define COLOR_CYAN      "\e[0;36m"
#define COLOR_LT_CYAN   "\e[1;36m"
#define COLOR_LT_GRAY   "\e[0;37m"
#define COLOR_WHITE     "\e[1;37m"
#define BHMAG "\e[1;95m"
#define REDHB "\e[0;101m"
#define YELHB "\e[0;103m"
#define BGRN "\e[1;32m"

#define CLEAR_SCREEN    "\e[2J\e[1;1H"

/*****************************
 * SYMBOLS FOR GAME ELEMENTS *
 *****************************/

#define S_FLOOR     " "
#define S_PLAYER    COLOR_RED "@" COLOR_OFF
#define S_WALL      COLOR_GRAY "#" COLOR_OFF
#define S_TARGET    COLOR_YELLOW "$" COLOR_OFF
#define S_INNER_WALL COLOR_CYAN "#" COLOR_OFF
#define S_MEDUZA COLOR_BLACK "*" COLOR_OFF
#define S_RIGHT COLOR_GREEN "[" COLOR_OFF
#define S_LEFT COLOR_GREEN "]" COLOR_OFF
#define S_DOOR BGRN "+" COLOR_OFF
#define S_KEY  BGRN  "&" COLOR_OFF
#define S_LIGHTER COLOR_YELLOW "L" COLOR_OFF
#define S_PORTAL COLOR_PURPLE "O" COLOR_OFF
#define S_SIDEWALL COLOR_PURPLE "-" COLOR_OFF
#define S_UPWALL COLOR_PURPLE "|" COLOR_OFF
#define S_SPACE COLOR_WHITE " " COLOR_OFF