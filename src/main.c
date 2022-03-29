#include <stdio.h>
#include <stdlib.h>
#include "field.h"

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + SX * (Y) + X))

#define XSIZE 5u
#define YSIZE 4u

void group_dump(Group* group_ptr);

int main(){

    field_node* field = malloc(sizeof(field_node));
    field -> prev = NULL;
    field -> next = NULL;
    field -> group_ptr = malloc(sizeof(Group));
    field -> group_ptr -> group_coord.x = 0;
    field -> group_ptr -> group_coord.y = 0;
    field -> group_ptr -> x_group_size = XSIZE;
    field -> group_ptr -> y_group_size = YSIZE;
    field -> group_ptr -> group_block = calloc(XSIZE * YSIZE, 1);

    for (int i = 0; i < 4; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i + 1, 1) = 1;
    }

    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 0, 3) = 1;
    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, XSIZE - 1, 0) = 1;

    group_dump(field -> group_ptr);

    group_resize(field -> group_ptr);
    printf("post resize: \n");
    group_dump(field -> group_ptr);

    field_free(&field);

    return 0;
}

void group_dump(Group* group_ptr){
    printf("global coord [%d][%d]\n", group_ptr -> group_coord.x, group_ptr -> group_coord.y);
    for (int y = (int) group_ptr -> y_group_size - 1; y >= 0;  y--) {
        for (unsigned int x = 0; x < group_ptr -> x_group_size; x++)
            printf("[%d]", COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y));
        printf("\n");
    }
}
