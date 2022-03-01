#include "field.h"
#include <stdlib.h>

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + SX * (Y) + X))

int neighbour_count(Group* block, unsigned int x, unsigned int y);

void field_free(Field* field_ptr){
    
    for(int i = 0; field_ptr[i] != NULL; i++){
        free(field_ptr[i] -> group_block);
        free(field_ptr[i]);    
    }

}

int group_step(Group* group_ptr){
    for(unsigned int i = 0; i < group_ptr -> x_group_size; i++){
        for(unsigned int j = 0; j < group_ptr -> y_group_size; j++){
            switch (neighbour_count(group_ptr, j, i)) {
                case 3 :
                    COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, j, i) = 1;
                    break;
                case 2 :
                    break;
                default :
                    COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, j, i) = 0;
                    break;
            }
        }
    }

    return 0;
}

int field_step(Field* field_state){

    for(int i = 0; field_state[i] != NULL; i++){
        group_step(field_state[i]);
    }

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

#undef GETVAL
