#include "field.h"
#include "analysis.h"
#include "draw.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ANALYSIS_DEPTH 10

unsigned int history_length(History* history);
int find_node_in_field(Field* field, field_node* node);
int analyze_oscillator(History history);
int analyze_glider(History history);
state_node* find_group_exact(Group_state state, Group* group, int x_offset, int y_offset);
state_node* find_group_moved(Group_state state, Group* group, int next);
int check_glider_to_inf(Group_state state, state_node* node);

Group_state copy_field_add_states(Field* src){
    field_node* cur_node = src[0];
    state_node* new_head = NULL;
    state_node* prev = NULL;

    while (cur_node != NULL) {
        state_node* new_node = (state_node*) malloc(sizeof(state_node));

        new_node -> prev = prev;
        new_node -> next = NULL;
        if (prev != NULL) {
            prev -> next = new_node;
        }

        if (cur_node -> prev == NULL)
            new_head = new_node;

        new_node -> status = unknown;
        new_node -> group_ptr = (Group*) malloc(sizeof(Group));
        memcpy(new_node -> group_ptr, cur_node -> group_ptr, sizeof(Group));
        unsigned int size = new_node -> group_ptr -> x_group_size * new_node -> group_ptr -> y_group_size;
        new_node -> group_ptr -> group_block = malloc(size);
        memcpy(new_node -> group_ptr -> group_block, cur_node -> group_ptr -> group_block, size);

        prev = new_node;
        cur_node = cur_node -> next;
    }

    return new_head;
}

int history_update(History* history, Field* field_ptr){
    hist_node* cur_hist = history[0];

    hist_node* new_node = (hist_node*) malloc(sizeof(hist_node));
    new_node -> prev = NULL;
    new_node -> next = cur_hist;
    if (cur_hist != NULL)
        cur_hist -> prev = new_node;
    new_node -> state = copy_field_add_states(field_ptr);
    history[0] = new_node;
    cur_hist = new_node;

    int length = 1;
    while (cur_hist -> next != NULL) {
        cur_hist = cur_hist -> next;
        length++;
    }

    if (length > ANALYSIS_DEPTH) { //if reached analysis depth in history size - clear oldest history entry
        cur_hist -> prev -> next = NULL;
        free_group_states(&cur_hist -> state);
        free(cur_hist); 
    }

    return 0;
}

int history_clear(History* history){
    if (history == NULL || history[0] == NULL)
        return 0;
    hist_node* cur_node = history[0];

    while (1) {
        free_group_states(&cur_node -> state);
        free(cur_node -> prev); 
        if (cur_node -> next == NULL) {
            free(cur_node);
            break;
        }
        cur_node = cur_node -> next;
    }

    *history = NULL;

    return 0;
}

unsigned int history_length(History* history){
    hist_node* cur_node = history[0];
    unsigned int length = 0;
    while (cur_node != NULL) {
        cur_node = cur_node -> next;
        length++;
    }
    return length;
}

state_node* find_group_exact(Group_state state, Group* group, int x_offset, int y_offset){
        
    state_node* cur_node = state;

    while (cur_node != NULL) {
        Group* cur_group = cur_node -> group_ptr;
        //printf("Comparing groups\n");
        //group_dump(cur_group);
        //group_dump(group);
        if ((cur_group -> group_coord.x == (group -> group_coord.x + x_offset)) &&
            (cur_group -> group_coord.y == (group -> group_coord.y + y_offset)) &&
            (cur_group -> x_group_size == group -> x_group_size) && 
            (cur_group -> y_group_size == group -> y_group_size) &&
            (!memcmp(cur_group -> group_block, group -> group_block, group -> x_group_size * group -> y_group_size)))
            return cur_node;


        cur_node = cur_node -> next;
    }
        
    return NULL;
}

state_node* find_group_moved(Group_state state, Group* group, int next){
    
    static int progress_save;

    if (!next)
        progress_save = 0;

    state_node* cur_node = state;

    int counter = 0;
    while (cur_node != NULL) {
        Group* cur_group = cur_node -> group_ptr;
        if ((cur_group -> x_group_size == group -> x_group_size) && 
            (cur_group -> y_group_size == group -> y_group_size) &&
            (!memcmp(cur_group -> group_block, group -> group_block, group -> x_group_size * group -> y_group_size))) {

            if (counter < progress_save)
                counter++;
            else {
                progress_save++;
                return cur_node;
            }
        }

        cur_node = cur_node -> next;
    }

    return NULL;
}

int analyze_oscillator(History history){
    Group_state cur_node = history -> state;
    if (history -> next == NULL)
        return -1;

    while (cur_node != NULL) {
        unsigned int period = 1;
        hist_node* older_gen = history -> next;
        while (older_gen != NULL) {
            state_node* target_node = find_group_exact(older_gen -> state, cur_node -> group_ptr, 0, 0);
            if (target_node != NULL) {
                if (period == 1) 
                    cur_node -> status = stable;
                else {
                    cur_node -> status = oscillator; //could add a secondary check if many false-positives
                }
                cur_node -> period = period;
                break;
            }
            period++;
            older_gen = older_gen -> next;        
        }
        cur_node = cur_node -> next;
    }

    return 0;
}

int analyze_glider(History history){
    Group_state cur_node = history -> state;
    if (history -> next == NULL)
        return -1;

    while (cur_node != NULL) {
        unsigned int period = 1;
        hist_node* older_gen = history -> next;
        while (older_gen != NULL) {

            state_node* target_node = find_group_moved(older_gen -> state, cur_node -> group_ptr, 0);

            while (target_node != NULL) {
                int x_offset = target_node -> group_ptr -> group_coord.x - cur_node -> group_ptr -> group_coord.x;    
                int y_offset = target_node -> group_ptr -> group_coord.y - cur_node -> group_ptr -> group_coord.y;    

                if (((x_offset != 0) || (y_offset != 0)) && (period * 2 < history_length(&history))) { //continue checks only if moved and if analysis depth allows
                    x_offset *= 2;
                    y_offset *= 2;
                    hist_node* two_period_gen = older_gen;
                    for (unsigned int i = 0; i < period; i++) //get a period older in history
                        two_period_gen = two_period_gen -> next;
                    target_node = find_group_exact(two_period_gen -> state, cur_node -> group_ptr, x_offset, y_offset); //check if glider was in correct place two periods ago
                    if (target_node != NULL) {
                        cur_node -> status = glider;
                        cur_node -> period = period;
                        cur_node -> x_speed = -x_offset;
                        cur_node -> y_speed = -y_offset;
                        //printf("Speed x = %d; y = %d\n", cur_node -> x_speed, cur_node -> y_speed);
                        break;
                    }
                }

                target_node = find_group_moved(older_gen -> state, cur_node -> group_ptr, 1);
            }


            
            period++;
            older_gen = older_gen -> next; 
        }

        cur_node = cur_node -> next;
    }

    return 0;
}

int field_step_analyzed(Field* field_ptr, History* history){
    field_step(field_ptr);
    history_update(history, field_ptr);

    analyze_state(*history);

    return 0;
}

int analyze_state(History history){
    analyze_oscillator(history);
    analyze_glider(history);
    //printf("________________\n");
    return 0;
}

int free_group_states(Group_state* state){
    if (state == NULL || state[0] == NULL)
        return 0;

    state_node* cur_node = state[0];
    while (cur_node -> next != NULL) {
       free(cur_node -> group_ptr -> group_block);
       free(cur_node -> group_ptr);
       cur_node = cur_node -> next;
       free(cur_node -> prev);
    }

    free(cur_node -> group_ptr -> group_block);
    free(cur_node -> group_ptr);

    free(cur_node);

    state[0] = NULL;
    return 0;
}

int check_evolve_finish(Group_state state){
    state_node* cur_node = state;

    while (cur_node != NULL) {
        if (cur_node -> status == unknown)
            break;

        if ((cur_node -> status == glider) && !check_glider_to_inf(state, cur_node))
            break;

        cur_node = cur_node -> next;
    }

    if (cur_node == NULL)
        return 1;
    else 
        return 0;

}

int check_glider_to_inf(Group_state state, state_node* node){ //really dumb and incorrect version

    int away_on_x = 0;
    int away_on_y = 0;

    if (((node -> group_ptr -> group_coord.x > 50) && (node -> x_speed >= 0)) ||
        ((node -> group_ptr -> group_coord.x < -50) && (node -> x_speed <=0)))
        away_on_x = 1;

    if (((node -> group_ptr -> group_coord.y > 50) && (node -> y_speed >= 0)) ||
        ((node -> group_ptr -> group_coord.y < -50) && (node -> y_speed <=0)))
        away_on_y = 1;

    if (away_on_x && away_on_y)
        return 1;
    else 
        return 0;


}
