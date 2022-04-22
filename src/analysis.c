#include "field.h"
#include "analysis.h"
#include <stdlib.h>
#include <string.h>

Field copy_field(Field* src){
    field_node* cur_node = src[0];
    field_node* new_head = NULL;
    field_node* prev = NULL;

    while (cur_node != NULL) {
        field_node* new_node = (field_node*) malloc(sizeof(field_node));

        new_node -> prev = prev;
        new_node -> next = NULL;
        if (prev != NULL) {
            prev -> next = new_node;
        }

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

    while (cur_hist -> next != NULL)
        cur_hist = cur_hist -> next;

    hist_node* new_node = (hist_node*) malloc(sizeof(hist_node));
    new_node -> prev = cur_hist;
    new_node -> next = NULL;
    cur_hist -> next = new_node;
    new_node -> field_state = copy_field(field_ptr);

    return 0;
}

int history_clear(History* history){
    hist_node* cur_node = history[0];

    while (1) {
        field_free(&cur_node -> field_state);
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

