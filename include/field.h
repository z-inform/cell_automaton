#pragma once 
#include <stdint.h>

struct coord_pair_struct {
    int x;
    int y;
};

typedef struct coord_pair_struct coord_pair;

struct ucoord_pair_struct {
    unsigned int x;
    unsigned int y;
};

typedef struct ucoord_pair_struct ucoord_pair;

struct group_struct{
    coord_pair group_coord;
    unsigned int x_group_size;
    unsigned int y_group_size;
    void* group_block;
};

typedef struct group_struct Group;

struct field_node_struct;
typedef struct field_node_struct field_node;

struct field_node_struct{
    Group* group_ptr;
    field_node* next;
    field_node* prev;
};

typedef field_node* Field;

int field_step(Field* field_ptr);
void field_free(Field* field_ptr);
int field_resplit(Field* field_ptr);
int group_step(Group* group_ptr); 
