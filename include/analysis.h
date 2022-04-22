#pragma once
#include "field.h"

enum States {stable, oscillator, glider, unknown};

typedef struct state_node {
    States status;
    field_node* node_ptr;
    state_node* prev;
    state_node* next;
} state_node;

typedef struct hist_node {
    Field field_state;
    hist_node* prev;
    hist_node* next;
} hist_node;

typedef state_node* Group_states;
typedef hist_node* History;

int history_update(History* history, Field* field_ptr);
int history_clear(History* history);
Field copy_field(Field* src);
