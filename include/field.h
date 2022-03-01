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

typedef Group* Field;

int field_step(Field* field_state);
void field_free(Field* field_ptr);
int field_resplit(Field* field_state);
int group_step(Group* group_ptr); 
