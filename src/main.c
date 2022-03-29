#include <stdio.h>
#include <stdlib.h>
#include "field.h"

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + SX * (Y) + X))

void group_dump(Group* group_ptr);

int main(){

    field_node* field = malloc(sizeof(field_node));
    field -> prev = NULL;
    field -> next = NULL;
    field -> group_ptr = malloc(sizeof(Group));
    field -> group_ptr -> group_coord.x = 0;
    field -> group_ptr -> group_coord.y = 0;
    field -> group_ptr -> x_group_size = 5u;
    field -> group_ptr -> y_group_size = 3u;
    field -> group_ptr -> group_block = calloc(15, 1);

    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i + 1, 1) = 1;
    }

    group_resize(field -> group_ptr);
    group_dump(field -> group_ptr);

    field_free(&field);

    return 0;
}

void group_dump(Group* group_ptr){
    for (int y = (int) group_ptr -> y_group_size - 1; y >= 0;  y--) {
        for (unsigned int x = 0; x < group_ptr -> x_group_size; x++)
            printf("[%d]", COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y));
        printf("\n");
    }
}
