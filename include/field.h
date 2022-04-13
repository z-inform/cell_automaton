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
int field_split(Field* field_ptr);
int field_merge(Field* field_ptr);
int group_step(Group* group_ptr); 
int group_resize(Group* group_ptr);
field_node* group_split(Field* field_ptr, field_node* node_ptr, unsigned int x, unsigned int y); 
field_node* group_merge(Field* field_ptr, field_node* cur_node, field_node* other_node);
Group* find_cell_group(Field* field_ptr, int x, int y);
