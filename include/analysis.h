#pragma once
#include "field.h"

enum States {stable, oscillator, glider, unknown};

typedef struct state_node {
    States status;
    unsigned int period;
    Group* group_ptr;
    state_node* prev;
    state_node* next;
} state_node;

typedef state_node* Group_state;

typedef struct hist_node {
    Group_state state;
    hist_node* prev;
    hist_node* next;
} hist_node;

typedef hist_node* History;

int history_update(History* history, Field* field_ptr);
int history_clear(History* history);
Group_state copy_field_add_states(Field* src);
int analyze_state(History* history);
int field_step_analyzed(Field* field_ptr, History* history);
int free_group_states(Group_state* state);
