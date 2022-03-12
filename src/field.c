#include "field.h"
#include <stdlib.h>

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + SX * (Y) + X))

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y);
int row_cells(Group* group_ptr, unsigned int y);
int column_cells(Group* group_ptr, unsigned int x);
int group_resize(Group* group_ptr);

void field_free(Field* field_ptr){
        
    field_node* cur_node = field_ptr[0];

    for(int i = 0; cur_node -> next != NULL; i++){
        free(cur_node -> group -> group_block);
        cur_node = cur_node -> next;
        free(cur_node -> prev);    
    }

    free(cur_node -> group -> group_block);
    free(cur_node -> prev);    

}

int group_step(Group* group_ptr){
    uint8_t* new_state = calloc(group_ptr -> x_group_size * group_ptr -> x_group_size, 1);
    for(unsigned int i = 0; i < group_ptr -> x_group_size; i++){
        for(unsigned int j = 0; j < group_ptr -> y_group_size; j++){
            switch (neighbour_count(group_ptr, i, j)) {
                case 3 :
                    COORDVAL(new_state, group_ptr -> x_group_size, i, j) = 1;
                    break;
                case 2 :
                    break;
                default :
                    COORDVAL(new_state, group_ptr -> x_group_size, i, j) = 0;
                    break;
            }
        }
    }

    free(group_ptr -> group_block);
    group_ptr -> group_block = new_state;

    return 0;
}

int field_step(Field* field_state){

    field_node* cur_node = field_ptr[0];

    for(int i = 0; cur_node != NULL; i++){
        group_step(cur_node -> group_ptr);
        cur_node = cur_node -> next;
    }

    field_resplit(Field* field_state);

    return 0;
}

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y){
    unsigned int count = 0;

    if (x != group_ptr -> x_group_size) {
        for(char i = -1; i <= 1; i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x + 1, y + i);
        }
    }

    if (x != 0) {
        for(char i = -1; i <= 1; i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x - 1, y + i);
        }
    }

    if (y != group_ptr -> y_group_size) {
        for(char i = -1; i <= 1; i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x + i, y + 1);
        }
    }

    if (y != 0) {
        for(char i = -1; i <= 1; i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x + i, y - 1);
        }
    }

    return count;
}

int row_cells(Group* group_ptr, unsigned int y){
    unsigned int count = 0;
    for(unsigned int i = 0; i < group_ptr -> x_group_size; i++){
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, y);
    }
}

int column_cells(Group* group_ptr, unsigned int x){
    unsigned int count = 0;
    for(unsigned int i = 0; i < group_ptr -> y_group_size; i++){
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, i);
    }
}

int group_resize(Group* group_ptr){
    int x_coord_start = group_ptr -> group_coord -> x;
    int y_coord_start = group_ptr -> group_coord -> y;
    unsigned int x_start = 0;
    unsigned int y_start = 0;
    unsigned int x_end = group_ptr -> x_group_size;
    unsigned int y_end = group_ptr -> y_group_size;
    

    if (row_cells(group_ptr, 0)){ //if alive cells on lower group boundary expand downwards
        group_ptr -> group_coord -> y -= 1;
        group_ptr -> y_group_size += 1;
    }

    if (row_cells(group_ptr, y_end)){ //if alive cells on upper group boundary expand upwards
        group_ptr -> y_group_size += 1;
    }

    if (column_cells(group_ptr, 0)){ //if alive cells on left group boundary expand left
        group_ptr -> group_coord -> x -= 1;
        group_ptr -> x_group_size += 1;
    }

    if (column_cells(group_ptr, x_end)){ //if alive cells on right group boundary expand right
        group_ptr -> x_group_size += 1;
    }


    for(unsigned int i = 1; !row_cells(group_ptr, i); i++){ //cut empty rows from the bottom
        group_ptr -> group_coord -> y += 1;
        group_ptr -> y_group_size -= 1;
        y_start += 1;
    }

    for(unsigned int i = 1; !row_cells(group_ptr, y_end - i); i++){ //cut empty rows from the top
        group_ptr -> y_group_size -= 1;
        y_end -= 1;
    }

    for(unsigned int i = 1; !column_cells(group_ptr, i); i++){ //cut empty rows from the left
        group_ptr -> group_coord -> x += 1;
        group_ptr -> x_group_size -= 1;
        x_start += 1;
    }

    for(unsigned int i = 1; !column_cells(group_ptr, x_end - i); i++){ //cut empty rows from the right
        group_ptr -> x_group_size -= 1;
        x_end -= 1;
    }

    unsigned int x_offset = (unsigned int) (x_coord_start - (group_ptr -> group_coord -> x));
    unsigned int y_offset = (unsigned int) (y_coord_start - (group_ptr -> group_coord -> y));

    if ((x_offset == 0) && (y_offset == 0) && //no need to realloc if field size has not changed
        ((x_end - x_start) == group_ptr -> x_group_size) &&
        ((y_end - y_start) == group_ptr -> y_group_size)){
        return 0;
    }

    void* new_block = calloc((group_ptr -> x_group_size) * (group_ptr -> y_group_size), 1);

    for(unsigned int i = x_start; i < x_end; i++){
        for(unsigned int j = y_start; j < y_end; j++){
            COORDVAL(new_block, group_ptr -> x_group_size, x_offset + i, y_offset + j) = COORDVAL(group_ptr -> group_block, x_end - x_start, i, j);
        }
    }

    free(group_ptr -> group_block);
    group_ptr -> group_block = new_block;

    return 0;
}

#undef GETVAL
