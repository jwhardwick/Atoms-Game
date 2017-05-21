#ifndef ATOMGAME_H
#define ATOMGAME_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_LINE 255
#define MIN_WIDTH 2
#define MIN_HEIGHT 2
#define MAX_WIDTH 255
#define MAX_HEIGHT 255
#define MIN_PLAYERS 2
#define MAX_PLAYERS 6

/* TO DO
UNDO functions
EXPANSION
how am i going to implement this shit.
1 - check if adding to atom_count causes it to go over
	limit
PLAYFROM
SAVE/LOAD
STAT
	- read through grid and return counts
*/


typedef enum { PRE_GAME, GAME_STARTED, FIRST_ROUND, AFTER_FIRST_ROUND, GAME_LOADED, VALID_PLAYFROM } game_states;

typedef struct index_t index_t;
typedef struct move_data_t move_data_t;
typedef struct grid_t grid_t;
typedef struct grid_data_t grid_data_t;
typedef struct stats_t stats_t;
typedef struct save_file_t save_file_t;

struct index_t {
	game_states current_game_state;
	int turn_counter;
	int turns_played;
	int number_of_players;
	int width;
	int height;
	int player_stats[6];
	char* whose_turn_is_it_storage;
	move_data_t* move_data_head;
	save_file_t* save_file;
	grid_data_t* grid_array[255][255];
};

struct move_data_t {
	int x;
	int y;
	move_data_t* next_turn;
};


struct grid_data_t {
	char player_color;
	int atom_count;
};

struct save_file_t {
	uint8_t width;
	uint8_t height;
	uint8_t no_players;
	uint32_t* raw_move_data;
};

int check_convert_int(char* str);
void whose_turn_is_it(index_t* index, int turn);
void initialise_grid(index_t* index);
void handle_help(void);
void free_grid_data(index_t* index);
void free_move_data(index_t* index);
void free_index(index_t* index);
void handle_quit(index_t* index);
void print_move_data(index_t* index);
void handle_display(index_t* index);
void handle_start(index_t* index);
void initialise_new_grid_data(index_t* index, int x, int y);
void handle_place(index_t* index);
void handle_undo(index_t* index);
void handle_stat(index_t* index);
void handle_load(index_t* index);
void handle_save(index_t* index);
int handle_playfrom(index_t* index, int loaded_turns_played);
void check_for_expansion(index_t* index, int x, int y);
void clear_move_data_after_explosion(index_t* index, int x, int y);
void trigger_explosion_corner(index_t* index, int x, int y);
void trigger_explosion_side(index_t* index, int x, int y);
void trigger_explosion_middle(index_t* index, int x, int y);
void add_move_to_grid(index_t* index, int x, int y);
void read_input(index_t* index);
void update_stats(index_t* index);
bool is_player_out(index_t* index);
int read_input_after_load(index_t* index, int loaded_turns_played);





#endif
