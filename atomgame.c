#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "atomgame.h"

int check_convert_int(char* str){
    // returns -1 if not a straight integer
    // returns the int if it's a valid int
    char *ptr;
    int result = 0;
    result = strtol(str, &ptr, 10);
    //printf("%s result = %d\n", str, result);
    if(*ptr){
        return -1;
    }
    return result;
}
void whose_turn_is_it(index_t* index, int turn) {
    // I NEED THIS TO HAVE index AND turn FOR EXPANSION
    int mod_players = 0;
    mod_players = turn % index->number_of_players;
    if(mod_players == 0){
        mod_players = index->number_of_players;
    }

    //printf("mod_players %d\n", mod_players);
    // need to find out how to return string values
    // maybe #define some strings?
    switch(mod_players){
        case 1:
            index->whose_turn_is_it_storage = "Red";
            break;
        case 2:
            index->whose_turn_is_it_storage = "Green";
            break;
        case 3:
            index->whose_turn_is_it_storage = "Purple";
            break;
        case 4:
            index->whose_turn_is_it_storage = "Blue";
            break;
        case 5:
            index->whose_turn_is_it_storage = "Yellow";
            break;
        case 6:
            index->whose_turn_is_it_storage = "White";
            break;
    }
}
void check_for_victory(index_t* index) {
    // scan over player_stats and see if only one remains
    int count = 0;
    for (int i = 0; i < index->number_of_players; i++) {
        if (index->player_stats[i] != 0) {
            count += 1;
        }
    }
    //printf("victory count = %d\n", count);
    if (count == 1) {
        //victory!
        // realising a flaw in this system.. to do with adding grids and deciding on their ownership
        // actually should be fine
        whose_turn_is_it(index, index->turn_counter);
        char* turn_ptr = index->whose_turn_is_it_storage;
        printf("%s Wins\n\n", turn_ptr);
        handle_quit(index);
    }

}
bool is_player_out(index_t* index) {
    // return True if the player is out
    int mod_players = 0;
    mod_players = index->turn_counter % index->number_of_players;
    if (mod_players == 0){
        mod_players = index->number_of_players;
    }
    mod_players -= 1;
    //printf("player_stats[%d] = %d\n", mod_players, index->player_stats[mod_players]);
    if (index->player_stats[mod_players] == 0) {
        return true;
    }
        return false;


}
void initialise_grid(index_t* index) {
    //printf("initialise_grid\n");
    for(int i = 0; i < index->height; i++){
        for(int j = 0; j < index->width; j++){
            index->grid_array[i][j] = NULL;
        }
    }

}
void handle_help(void) {
    printf("HELP displays this help message\n");
    printf("QUIT quits the current game\n\n");
    printf("DISPLAY draws the game board in terminal\n");
    printf("START <number of players> <width> <height> starts the game\n");
    printf("PLACE <x> <y> places an atom in a grid space\n");
    printf("UNDO undoes the last move made\n");
    printf("STAT displays game statistics\n\n");
    printf("SAVE <filename> saves the state of the game\n");
    printf("LOAD <filename> loads a save file\n");
    printf("PLAYFROM <turn> plays from n steps into the game\n");
}
void free_grid_data(index_t* index) {
    for(int i = 0; i < index->width; i++) {
        for(int j = 0; j < index->height; j++) {
            //read grid_array i j
            // if not null, free
            if(index->grid_array[i][j] != NULL) {
                free(index->grid_array[i][j]);
            }
        }
    }
}
void free_move_data(index_t* index) {
    //go through move data
    move_data_t* temp;
    move_data_t* reader;
    reader = index->move_data_head;
    while(reader != NULL) {
        //wipe data
        temp = reader->next_turn;
        free(reader);
        reader = temp;
    }
}
void free_index(index_t* index) {
    free(index);
    exit(1);
}
void handle_quit(index_t* index) {
    //printf("handle_quit\n");
    // need to free the shit out of this program.
    // go through grid_array and the assosciated data
    // go through stats_t
    // free move_data_t completely
    // free index contents
    if (index->current_game_state == PRE_GAME || index->current_game_state == GAME_LOADED) {
        free_index(index);
        return;
    }
    if (index->current_game_state == GAME_STARTED) {
        free_grid_data(index);
        free_index(index);
        return;
    }
    free_grid_data(index);
    free_move_data(index);
    free_index(index);
}
void print_move_data(index_t* index) {
    // prints move data
    int counter = 1;
    move_data_t* move_data_reader;
    move_data_reader = index->move_data_head;
    for( ; ; ){
        printf("turn = %d, x = %d, y = %d, next = %p\n", counter, move_data_reader->x, move_data_reader->y, move_data_reader->next_turn);
        move_data_reader = move_data_reader->next_turn;
        counter += 1;
        if(move_data_reader == NULL){
            break;
        }
    }
}
void handle_display(index_t* index) {
    //printf("handle_display\n");
    printf("\n+");
    for(int i = 0; i < ((3 * index->width) - 1); i++){
        printf("-");
    }
    printf("+\n");
    for(int i = 0; i < index->height; i++){
        for(int j = 0; j < index->width; j++){
            printf("|");
            if(index->grid_array[i][j] == NULL){
                printf("  ");
            }
            else{
                printf("%c", index->grid_array[i][j]->player_color);
                printf("%d", index->grid_array[i][j]->atom_count);
            }
        }
        printf("|\n");
    }
    printf("+");
    for(int i = 0; i < ((3 * index->width) - 1); i++){
        printf("-");
    }
    printf("+\n\n");

}
void start_game(index_t* index, int players, int width, int height) {
    index->turn_counter = 1;
    index->turns_played = 1;
    index->number_of_players = players;
    index->width = width;
    index->height = height;
    // build grid
    initialise_grid(index);

}
void handle_start(index_t* index) {
    //printf("handle_start\n");
    if(index->current_game_state != PRE_GAME) {
        printf("Invalid Command, game already started\n");
        return;
    }
    int players = 0;
    int width = 0;
    int height = 0;
    char* pptr;
    char* wptr;
    char* hptr;
    char* test;

    pptr = strtok(NULL, " \n");
    //printf("pptr = %s\n", pptr);
    if(pptr == NULL){
        printf("Missing Argument\n");
        return;
    }
    players = check_convert_int(pptr);

    wptr = strtok(NULL, " ");
    //printf("wptr = %s\n", wptr);
    if(wptr == NULL){
        printf("Missing Argument\n");
        return;
    }
    width = check_convert_int(wptr);

    hptr = strtok(NULL, " \n");
    //printf("hptr = %s\n", hptr);
    if(hptr == NULL){
        printf("Missing Argument\n");
        return;
    }
    height = check_convert_int(hptr);

    //printf("players == %d, width == %d, height == %d\n", players, width, height);
    test = strtok(NULL, " \n");
    if(test != NULL){
        printf("Too Many Arguments\n");
        return;
    }else if(width < MIN_WIDTH || width > MAX_WIDTH){
        printf("Invalid width. Range is 2-255\n");
        return;
    }else if(height < MIN_HEIGHT || height > MAX_HEIGHT){
        printf("Invalid height. Range is 2-255\n");
        return;
    }else if(width <= 0 || height <= 0 || players < MIN_PLAYERS || players > MAX_PLAYERS){
        printf("Invalid command arguments\n");\
        return;
    }else if( (width * height) < players){
        printf("Cannot Start Game\n");
        return;
    }

    // add variables to struct data
    // this game state is for fresh game starts
    index->current_game_state = GAME_STARTED;
    start_game(index, players, width, height);
    whose_turn_is_it(index, index->turn_counter);
    printf("Game Ready\n");
    printf("%s's Turn\n\n", index->whose_turn_is_it_storage);
}
void initialise_new_grid_data(index_t* index, int x, int y) {
    //printf("initialise_new_grid_data\n");
    grid_data_t* new_grid_data;
    new_grid_data = NULL;
    new_grid_data = malloc(sizeof(grid_data_t*));
    if(new_grid_data == NULL) {
        printf("malloc failed\n");
    }
    whose_turn_is_it(index, index->turn_counter);
    new_grid_data->player_color = *index->whose_turn_is_it_storage;
    new_grid_data->atom_count = 1;
    index->grid_array[y][x] = new_grid_data;
}
void add_place_data(index_t* index, int x, int y) {

    whose_turn_is_it(index, index->turn_counter);
    char* new_move_player_color = index->whose_turn_is_it_storage;

    //printf("turn = %d, [ %d, %d ]\n", index->turn_counter, x, y);

    // move is valid if grid_array[y][x] == NULL;
    // or player color is equal
    if(index->grid_array[y][x] == NULL){
        //printf("NULL pointer, move is valid!\n");
        initialise_new_grid_data(index, x, y);
    }
    else if(index->grid_array[y][x]->player_color == new_move_player_color[0]) {
        //printf("%c owns this square, %c can place here!\n", index->grid_array[y][x]->player_color, new_move_player_color[0]);
        // send to another function to test if there will be an explosion
        //printf("sending to check for expansion\n");
        check_for_expansion(index, x, y);

    }
    else if(index->grid_array[y][x]->player_color != new_move_player_color[0]) {
        //printf("%c owns this square, %c cannot place here!\n", index->grid_array[y][x]->player_color, new_move_player_color[0]);
        return;
    }
    move_data_t* new_move_data;
    new_move_data = NULL;
    new_move_data = malloc(sizeof(move_data_t));
    if(new_move_data == NULL) {
        printf("malloc failed\n");
    }
    new_move_data->x = x;
    new_move_data->y = y;
    new_move_data->next_turn = NULL;

    if(index->current_game_state == GAME_STARTED){
        index->move_data_head = new_move_data;
    }
    // add to existing move data
    else if(index->current_game_state == FIRST_ROUND || index->current_game_state == AFTER_FIRST_ROUND){
        // loop through index->move_data_head until == NULL
        // add move_data to end
        move_data_t* move_data_reader;
        move_data_reader = index->move_data_head;
        for( ; ; ){
            if(move_data_reader->next_turn == NULL){
                move_data_reader->next_turn = new_move_data;
                break;
            }else{
                move_data_reader = move_data_reader->next_turn;
            }
        }
        // we're now at the end of the most recent move_data addition
    }
        //printf("turn_counter = %d\n", index->turn_counter);
    if (index->turn_counter <= index->number_of_players) {
        index->current_game_state = FIRST_ROUND;
    }
    else if (index->turn_counter > index->number_of_players) {
        index->current_game_state = AFTER_FIRST_ROUND;
    }

    // check for victory
    if (index->current_game_state == AFTER_FIRST_ROUND) {
        //printf("checking for victory!\n");
        update_stats(index);
        check_for_victory(index);
    }
    // this is the part for new grid data
    index->turn_counter += 1;
    index->turns_played += 1;
    update_stats(index);



    // check for if someone is out, if they are, turn_counter +=1
    if (index->current_game_state == AFTER_FIRST_ROUND) {
        while (is_player_out(index)) {
            //printf("player is out!\n");
            index->turn_counter += 1;
        }
    }


}
void handle_place(index_t* index) {
    int x = 0;
    int y = 0;
    char* xptr;
    char* yptr;
    char* test;
    xptr = strtok(NULL, " ");
    //printf("xptr = %s\n", xptr);
    if(xptr == NULL){
        printf("Invalid Coordinates1\n");
        return;
    }
    x = check_convert_int(xptr);

    yptr = strtok(NULL, " \n");
    //printf("yptr = %s\n", yptr);
    if(yptr == NULL){
        printf("Invalid Coordinates2\n");
        return;
    }
    y = check_convert_int(yptr);
    //printf("x == %d, y == %d\n", x, y);
    test = strtok(NULL, " \n");
    if(test != NULL || x < 0 || y < 0 || x >= index->width || y >= index->height){
        printf("Invalid Coordinates3\n");
        return;
    }
    add_place_data(index, x, y);
    whose_turn_is_it(index, index->turn_counter);
    char* turn_ptr = index->whose_turn_is_it_storage;
    printf("%s's Turn\n\n", turn_ptr);

}
void reset_to_turn(index_t* index) {

}
void delete_end_move_data(move_data_t* move_data_head) {
    //printf("delete_end_move_data\n");
    move_data_t* move_data_reader = move_data_head;
    move_data_t* temp = move_data_reader;
    for ( ; ; ) {

        if (move_data_reader->next_turn == NULL) {
            // we are on the last turn
            free(move_data_reader);
            temp->next_turn = NULL;
            break;
            // memory error here on second turn case.

        }
        temp = move_data_reader;
        move_data_reader = move_data_reader->next_turn;
    }
    //printf("delete_end_move_data complete\n");



}
move_data_t* copy_move_data(index_t* index) {
    // reads out all move data up to here DEBUGGG
    // move_data_t* move_data_x = index->move_data_head;
    // int countx = 1;
    // while (move_data_x != NULL) {
    //     printf("turn = %d, [ X, Y ] = [ %d, %d ]\n", countx, move_data_x->x, move_data_x->y);
    //     move_data_x = move_data_x->next_turn;
    //     countx = countx + 1;
    // }
    // end DEBUGGG


    // copy exisitng move data
    move_data_t* move_data_copy = NULL;
    move_data_copy = malloc(sizeof(move_data_t));
    if(move_data_copy == NULL){
        printf("malloc failed\n");
    }
    move_data_t* move_data_copy_head = move_data_copy;

    move_data_t* move_data_reader = index->move_data_head;
    //int countz = 1;
    for ( ; ; ) {

        move_data_copy->x = move_data_reader->x;
        move_data_copy->y = move_data_reader->y;
        move_data_copy->next_turn = move_data_reader->next_turn;

        // printf("turn = %d, [ X, Y ] = [ %d, %d ]\n", countz, move_data_copy->x, move_data_copy->y);
        // countz += 1;

        //free(move_data_reader); --- keeping this seperate
        move_data_reader = move_data_copy->next_turn;

        if (move_data_reader == NULL) {
            break;
        }

        // if we're up to here we know we'll need a new move_data_t to hold the next copy
        move_data_t* new_move_data_copy = NULL;
        new_move_data_copy = malloc(sizeof(move_data_t));
        if(new_move_data_copy == NULL){
            printf("malloc failed\n");
        }
        move_data_copy->next_turn = new_move_data_copy;
        move_data_copy = new_move_data_copy;

    }

    // reads out move_data_copy to see it's copied.
    // move_data_t* move_data_copy_reader = move_data_copy_head;
    // int count = 1;
    // printf("\ncopy_reader:\n");
    // while (move_data_copy_reader != NULL) {
    //     printf("turn = %d, [ X, Y ] = [ %d, %d ]\n", count, move_data_copy_reader->x, move_data_copy_reader->y);
    //     move_data_copy_reader = move_data_copy_reader->next_turn;
    //     count = count + 1;
    // }

    return move_data_copy_head;
}
void handle_undo(index_t* index) {
    //printf("handle_undo\n");
    if (index->current_game_state == PRE_GAME || index->current_game_state == GAME_LOADED
        || index->current_game_state == GAME_STARTED) {
        printf("Error, no move to undo\n");
        return;
    }

    if (index->turn_counter == 2) {
        //edge case for setting back to turn 1
        //printf("turn 2 edge case = true\n");
        // we should just reset the game completely
        int players = index->number_of_players;
        int width = index->width;
        int height = index->height;
        free_move_data(index);
        free_grid_data(index);
        start_game(index, players, width, height);
        index->current_game_state = GAME_STARTED;
        start_game(index, players, width, height);
        whose_turn_is_it(index, index->turn_counter);
        printf("%s's Turn\n\n", index->whose_turn_is_it_storage);
        return;
    }

    move_data_t* move_data_copy_head = copy_move_data(index);
    free_move_data(index);
    // Now i have a copy of move data, time to refresh grid board
    free_grid_data(index);
    initialise_grid(index);
    //now i want to send every x,y turn by turn to

    //need to remove last move from move_data_copy
    // for PLAYFROM, i can use the same function

    delete_end_move_data(move_data_copy_head);

    index->turn_counter = 1;
    index->turns_played = 1;
    index->current_game_state = GAME_STARTED;
    update_stats(index);

    //add_place_data(index_t* index, int x, int y);

    //read through move_data_copy and make turns

    move_data_t* move_data_copy_move_maker = move_data_copy_head;
    //int count = 1;
    //printf("\nmove_maker:\n");
    for( ; ; ) {
        //printf("turn = %d, [ X, Y ] = [ %d, %d ], next = %p\n", count, move_data_copy_move_maker->x, move_data_copy_move_maker->y, move_data_copy_move_maker->next_turn);
        add_place_data(index, move_data_copy_move_maker->x, move_data_copy_move_maker->y);
        if (move_data_copy_move_maker->next_turn == NULL) {
            break;
        }
        move_data_copy_move_maker = move_data_copy_move_maker->next_turn;
        //count = count + 1;
    }

    whose_turn_is_it(index, index->turn_counter);
    char* turn_ptr = index->whose_turn_is_it_storage;
    printf("%s's Turn\n\n", turn_ptr);
    return;

    // i might have to add an edge case to undo move data if only one turn has been played
}
void update_stats(index_t* index) {
    // called at the end of every turn
    // reads through board and updates stats that are stored in index->player_stats

    //reset stats to 0
    for(int i = 0; i < 6; i++){
        index->player_stats[i] = 0;
        //printf("players%d = %d\n", i, players[i]);
    }
    // read through index->grid and count scores
    for(int i = 0; i < index->height; i++) {
        for(int j = 0; j < index->width; j++){
            if(index->grid_array[i][j] != NULL) {
                char color = index->grid_array[i][j]->player_color;
                // printf("color = %c\n", color);
                int count = index->grid_array[i][j]->atom_count;
                // printf("count = %d\n", count);
                switch(color){
                    case 'R':
                        index->player_stats[0] += count;
                        break;
                    case 'G':
                        index->player_stats[1] += count;
                        break;
                    case 'P':
                        index->player_stats[2] += count;
                        break;
                    case 'B':
                        index->player_stats[3] += count;
                        break;
                    case 'Y':
                        index->player_stats[4] += count;
                        break;
                    case 'W':
                        index->player_stats[5] += count;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
void handle_stat(index_t* index) {
    // up and running
    // need to add response for if a player has already quit
    // will implement after handle_quit
    if (index->current_game_state == PRE_GAME) {
        printf("Game Not In Progress\n");
        return;
    }
    update_stats(index);

    for(int i = 0; i < index->number_of_players; i++) {
        whose_turn_is_it(index, i+1);
        char* turn_ptr = index->whose_turn_is_it_storage;
        printf("Player %s:\n", turn_ptr);

        if (index->current_game_state == GAME_STARTED || index->current_game_state == FIRST_ROUND || index->current_game_state == GAME_LOADED) {
            printf("Grid Count: %d\n", index->player_stats[i]);
        }
        else if(index->current_game_state == AFTER_FIRST_ROUND) {
            if (index->player_stats[i] == 0) {
                printf("Lost\n");
            }
            else {
                printf("Grid Count: %d\n", index->player_stats[i]);
            }
        }
        if (i < (index->number_of_players - 1)) {
            printf("\n");
        }
    }
    printf("\n");

    //printf("handle_stat\n");
}
void handle_load(index_t* index) {
    if (index->current_game_state != PRE_GAME) {
        printf("Restart Application To Load Save\n");
        return;
    }
    // find name of file to load
    char* loadptr = NULL;
    loadptr = strtok(NULL, " \n");
    //printf("loadptr = %s\n", loadptr);
    if(loadptr == NULL){
        printf("Missing Argument\n");
        return;
    }

    char* test = NULL;
    test = strtok(NULL, " \n");
    if(test != NULL){
        printf("Too Many Load Arguments\n");
        return;
    }

    FILE* load_file = NULL;
    load_file = fopen(loadptr, "r");
	if (load_file == NULL) {
		printf("Cannot Load Save\n");
		return;
	}

    // find out size of file
    fseek(load_file, 0L, SEEK_END);
    int file_size = ftell(load_file);
    rewind(load_file);
    printf("file_size = %d\n", file_size);

    // create array to store the loaded data
    uint8_t loaded_moves[file_size];
    fread(loaded_moves, sizeof(uint8_t), file_size, load_file);
    fclose(load_file);
    printf("Game Loaded\n");
    index->current_game_state = GAME_LOADED;

    for(int i = 0; i < file_size; i++) {
        printf("%d\n", loaded_moves[i]);
    }

    // will need to store this loaded data on the heap for next turn.
    // alternatively I just copy this into move_data now.

    // i need to initiliase the Game
    // find out how many turns there are = file_size - 3 / 4
    int loaded_turns_played = (file_size - 3) / 4;
    //printf("loaded turns = %d\n", loaded_turns_played);
    int requested_turn = 0;
    while (index->current_game_state == GAME_LOADED) {
        requested_turn = read_input_after_load(index, loaded_turns_played);
    }

    printf("requested_turn = %d\n", requested_turn);
    // sick! we're live. Now we can just load up to this turn

    // now initialise the game
    int width = loaded_moves[0];
    int height = loaded_moves[1];
    int players = loaded_moves[2];

    // test loaded parameters to ensure validity
    if(width < MIN_WIDTH || width > MAX_WIDTH){
        printf("Invalid width. Save file is corrupt\n");
        return;
    }else if(height < MIN_HEIGHT || height > MAX_HEIGHT){
        printf("Invalid height. Save file is corrupt\n");
        return;
    }else if(width <= 0 || height <= 0 || players < MIN_PLAYERS || players > MAX_PLAYERS){
        printf("Invalid start parameters. Save file is corrupt\n");\
        return;
    }else if( (width * height) < players){
        printf("Cannot Load Game. Save file is corrupt\n");
        return;
    }

    start_game(index, players, width, height);

    // now play the game!
    index->current_game_state = GAME_STARTED;
    for (int i = 0 ; i < requested_turn; i++) {
        //printf("i = %d, [ X, Y ] = [ %d, %d ]\n", i, loaded_moves[3 + (i * 4)], loaded_moves[4 + (i * 4)]);
        add_place_data(index, loaded_moves[3 + (i * 4)], loaded_moves[4 + (i * 4)]);
    }

    printf("Game Ready\n");
    whose_turn_is_it(index, index->turn_counter);
    char* turn_ptr = index->whose_turn_is_it_storage;
    printf("%s's Turn\n\n", turn_ptr);
    return;




}
void handle_save(index_t* index) {
    // ******** need to read in SAVE file name

    char* saveptr = NULL;
    saveptr = strtok(NULL, " \n");
    //printf("loadptr = %s\n", loadptr);
    if(saveptr == NULL){
        printf("Missing Save Argument\n");
        return;
    }

    char* test = NULL;
    test = strtok(NULL, " \n");
    if(test != NULL){
        printf("Too Many Save Arguments\n");
        return;
    }

    //read through move_data, store values as uint8_t
    uint8_t saved_moves[(index->turns_played - 1) * 4];
    uint8_t header[3];
    header[0] = index->width;
    header[1] = index->height;
    header[2] = index->number_of_players;

    // read move data to an array of uint8_t
    move_data_t* reader = index->move_data_head;
    //printf("turns_played%d\n", index->turns_played);
    for (int i = 0; i < (index->turns_played - 1); i++) {
        saved_moves[(4 * i) + 0] = reader->x;
        saved_moves[(4 * i) + 1] = reader->y;
        saved_moves[(4 * i) + 2] = 0;
        saved_moves[(4 * i) + 3] = 0;
        if (reader->next_turn == NULL) {
            break;
        }
        reader = reader->next_turn;
    }
    if (0 == access(saveptr, 0)) {
        printf("File Already Exists\n");
        return;
    }

    FILE* save_file = NULL;
    save_file = fopen(saveptr, "w");
	if (save_file == NULL) {
		perror("Unable to open file for writing");
		return;
	}
	fwrite(header, sizeof(uint8_t), 3, save_file);
	fwrite(saved_moves, sizeof(uint8_t), ((index->turns_played - 1) * 4), save_file);
	fclose(save_file);

    handle_load(index);
}
int handle_playfrom(index_t* index, int loaded_turns_played) {
    char* playptr = NULL;
    playptr = strtok(NULL, " \n");
    //printf("playptr = %s\n", playptr);
    if(playptr == NULL){
        printf("Missing Argument\n");
        return 0;
    }
    char* test = NULL;
    test = strtok(NULL, "\n");
    if(test != NULL){
        printf("Too Many Load Arguments\n");
        return 0;
    }

    if(strcmp(playptr, "END\n") == 0){
        return loaded_turns_played;
    }

    int requested_turn = 0;
    requested_turn = check_convert_int(playptr);
    // and <= number of moves made
    if (requested_turn <= 0) {
        printf("Invalid Turn Number\n");
        return 0;
    }
    if (requested_turn > loaded_turns_played) {
        return loaded_turns_played;
    }
    return requested_turn;
}
void check_for_expansion(index_t* index, int x, int y) {
    // function also adds to atom_count.

    // check if the grid square is already at the max
    // if no, then atom_count += 1 and gtfo
    // if yes, trigger explosion
    // corner = 2, side = 3, else = 4;
    // so max = above -1;
    if( (x < 0) || (x >= index->width) || (y < 0) || (y >= index->height)) {
        // pretty sure this test is redundant as I now do corners/sides case by case
        // thus no chance of going off the game board
        return;
    }
    //printf("checking for expansion\n");

    // this test checks if the grid square has no move in it
    if(index->grid_array[y][x] == NULL) {
        //printf("null pointer found\n");
        // if no grid, create data for that grid square
        initialise_new_grid_data(index, x, y);
        return;
    }

    //reject negatives or board limits


    // corner testing
    if( (x == 0 && y == 0) || (x == 0 & y == (index->height - 1)) || (x == (index->width - 1) && y == 0) || (x == (index->width -1) && y == (index->height -1)) ) {
        //printf("corner x = %d y = %d\n", x, y);
        if(index->grid_array[y][x]->atom_count == 1) {
            clear_move_data_after_explosion(index, x, y);
            trigger_explosion_corner(index, x, y);
            update_stats(index);
            check_for_victory(index);
            return;
        }
    }
    // side testing
    else if( (x == 0) || (x == index->width - 1) || (y == 0) || (y == index->height - 1) ) {
        //printf("side x = %d y = %d\n", x, y);
        if(index->grid_array[y][x]->atom_count == 2) {
            clear_move_data_after_explosion(index, x, y);
            trigger_explosion_side(index, x, y);
            update_stats(index);
            check_for_victory(index);
            return;
        }
    }
    // else - must be neither side nor corner
    else{
        if(index->grid_array[y][x]->atom_count == 3) {
            clear_move_data_after_explosion(index, x, y);
            trigger_explosion_middle(index, x, y);
            update_stats(index);
            check_for_victory(index);
            return;
        }
    }

    // if no explosion is triggered, grid_data is updated
    // makes sure the player_color is whoevers turn it is (for explosions)
    index->grid_array[y][x]->atom_count += 1;
    whose_turn_is_it(index, index->turn_counter);
    index->grid_array[y][x]->player_color = *index->whose_turn_is_it_storage;
    return;
}
void clear_move_data_after_explosion(index_t* index, int x, int y) {
    // need to clear up exisiting grid data, free it etc.
    // i malloc'd grid_data_t
    free(index->grid_array[y][x]);
    index->grid_array[y][x] = NULL;





}
void trigger_explosion_corner(index_t* index, int x, int y) {
    // we already know its a corner, so explosion will be 2.
    // easiest to just do individual testing for each cases
    // i.e ( x0, y0 ), ( x(w-1), y0 ), ( x0, y(h-1) ), ( x(w-1), y(h-1) )

    if (x == 0 && y == 0) {
        // top left - right,down
        check_for_expansion(index, (x + 1), y); // right
        check_for_expansion(index, x, (y + 1)); // down
    }
    else if (x == 0 && y == (index->height - 1)) {
        // bottom left - up, right
        check_for_expansion(index, x, (y - 1)); // up
        check_for_expansion(index, (x + 1), y); // right
    }
    else if (x == (index->width - 1) && y == 0) {
        // top right - down, left
        check_for_expansion(index, x, (y + 1)); // down
        check_for_expansion(index, (x - 1), y); // left
    }
    else if (x == (index->width - 1) && y == (index->height - 1)) {
        // bottom right - left, up
        check_for_expansion(index, (x - 1), y); // left
        check_for_expansion(index, x, (y - 1)); // up
    }

}
void trigger_explosion_side(index_t* index, int x, int y) {
    // left x == 0, right x == w-1, up y == 0, down y == h-1

    // need to clear up exisiting grid data, free it etc.

    if (x == 0) {
        //left - up, right, down
        check_for_expansion(index, x, (y - 1)); // up
        check_for_expansion(index, (x + 1), y); // right
        check_for_expansion(index, x, (y + 1)); // down
    }
    else if (x == (index->width - 1)) {
        //right - up, down, left
        check_for_expansion(index, x, (y - 1)); // up
        check_for_expansion(index, x, (y + 1)); // down
        check_for_expansion(index, (x - 1), y); // left
    }
    else if (y == 0) {
        //top - right, down, left
        check_for_expansion(index, (x + 1), y); // right
        check_for_expansion(index, x, (y + 1)); // down
        check_for_expansion(index, (x - 1), y); // left
    }
    else if (y == (index->height - 1)) {
        //bottom - up, right, left
        check_for_expansion(index, x, (y - 1)); // up
        check_for_expansion(index, (x + 1), y); // right
        check_for_expansion(index, (x - 1), y); // left
    }

}
void trigger_explosion_middle(index_t* index, int x, int y) {
    // always up, right, down, left

    check_for_expansion(index, x, (y - 1)); // up
    check_for_expansion(index, (x + 1), y); // right
    check_for_expansion(index, x, (y + 1)); // down
    check_for_expansion(index, (x - 1), y); // left
}
void add_move_to_grid(index_t* index, int x, int y) {
    // update move_data
    // update grid
}
void read_input(index_t* index) {
    char buffer[5000];
    fgets(buffer, sizeof(buffer), stdin);
    char* command;
    command = strtok(buffer, " ");

    if(strcmp(command, "HELP\n") == 0){
        //printf("help\n");
        handle_help();
    }
    else if(strcmp(command, "QUIT\n") == 0){
        //printf("quit\n");
            printf("Bye!\n");
        handle_quit(index);
    }
    else if(strcmp(command, "DISPLAY\n") == 0){
        //printf("display\n");
        handle_display(index);
    }
    else if(strcmp(command, "START") == 0){
        //printf("start\n");
        handle_start(index);
    }
    else if(strcmp(command, "PLACE") == 0){
        //printf("place\n");
        handle_place(index);
    }
    else if(strcmp(command, "UNDO\n") == 0){
        //printf("undo\n");
        handle_undo(index);
    }
    else if(strcmp(command, "STAT\n") == 0){
        //printf("stat\n");
        handle_stat(index);
    }
    else if(strcmp(command, "SAVE") == 0){
        //printf("save\n");
        handle_save(index);
    }
    else if(strcmp(command, "LOAD") == 0){
        //printf("load\n");
        handle_load(index);
    }
    else if(strcmp(command, "PLAYFROM") == 0){
        printf("Invalid Command\n");
    }
    else{
        printf("Invalid input, type HELP for instructions\n");
    }

}
int read_input_after_load(index_t* index, int loaded_turns_played) {
    char buffer[5000];
    fgets(buffer, sizeof(buffer), stdin);
    char* command;
    command = strtok(buffer, " ");

    if(strcmp(command, "QUIT\n") == 0){
        //printf("quit\n");
            printf("Bye!\n");
        handle_quit(index);
        return 0;
    }else if(strcmp(command, "PLAYFROM") == 0){
        //printf("playfrom\n");
        // handle_playfrom returns the requested to playfrom
        int playfrom_result = 0;
        playfrom_result = handle_playfrom(index, loaded_turns_played);
        if (playfrom_result > 0) {
            index->current_game_state = VALID_PLAYFROM;
            return playfrom_result;
        }

    }else{
        printf("Invalid Command\n");
        return 0;
    }
    return 0;
}

int main() {

    index_t* index_main;
    index_main = NULL;
    index_main = malloc(sizeof(index_t));
    if(index_main == NULL){
        printf("malloc failed\n");
    }
    // copy of index_main
    index_t* index;
    index = NULL;
    index = index_main;


    index->current_game_state = PRE_GAME;

    for( ; ; ) {
        read_input(index);
    }



    return 0;
}
