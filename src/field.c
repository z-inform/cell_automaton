#include "field.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + (SX) * (Y) + X))
#define MAX(X, Y) ((X > Y) ? (X) : (Y))
#define MIN(X, Y) ((X < Y) ? (X) : (Y))

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y);
int row_cells(Group* group_ptr, unsigned int y);
int column_cells(Group* group_ptr, unsigned int x);
uint8_t global_cell_status(Field* field_ptr, int x, int y);
int global_neighbour_count(Field* field_ptr, int x, int y);
int check_common_borders(Field* field_ptr, Group* cur_group, Group* other_group);
field_node* check_intersections(Field* field_ptr, field_node* cur_node);

void field_free(Field* field_ptr){

    if (field_ptr[0] == NULL)
        return;
        
    field_node* cur_node = field_ptr[0];

    for(int i = 0; cur_node -> next != NULL; i++){
        free(cur_node -> group_ptr -> group_block);
        free(cur_node -> group_ptr);
        cur_node = cur_node -> next;
        free(cur_node -> prev);    
    }

    free(cur_node -> group_ptr -> group_block);
    free(cur_node -> group_ptr);

    free(cur_node);

    field_ptr[0] = NULL;
}

int group_step(Group* group_ptr){
    uint8_t* new_state = (uint8_t*) calloc(group_ptr -> x_group_size * group_ptr -> y_group_size, 1);

    for(unsigned int i = 0; i < group_ptr -> x_group_size; i++){
        for(unsigned int j = 0; j < group_ptr -> y_group_size; j++){
            switch (neighbour_count(group_ptr, i, j)) {
                case 3 :
                    COORDVAL(new_state, group_ptr -> x_group_size, i, j) = 1;
                    break;
                case 2 :
                    COORDVAL(new_state, group_ptr -> x_group_size, i, j) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, j);
                    break;
                default :
                    COORDVAL(new_state, group_ptr -> x_group_size, i, j) = 0;
                    break;
            }
        }
    }

    free(group_ptr -> group_block);
    group_ptr -> group_block = new_state;
    return group_resize(group_ptr);

}

int field_step(Field* field_ptr){

    field_node* cur_node = field_ptr[0];

    if (cur_node == NULL)
        return -1;

    for(int i = 0; cur_node != NULL; i++){
        if (group_step(cur_node -> group_ptr) == 1) {
            free(cur_node -> group_ptr -> group_block);
            free(cur_node -> group_ptr);

            if (cur_node -> next != NULL)
                cur_node -> next -> prev = cur_node -> prev;
            if (cur_node -> prev != NULL)
                cur_node -> prev -> next = cur_node -> next;

            field_node* to_free = cur_node;
            if (cur_node == field_ptr[0]) 
                *field_ptr = cur_node -> next;
            cur_node = cur_node -> next;
            free(to_free);
        } else {
            cur_node = cur_node -> next;
        }
    }

    if (field_ptr == NULL)
        return -1;

    field_split(field_ptr);
    field_merge(field_ptr);

    return 0;
}

int group_resize(Group* group_ptr){

    int x_coord_new = group_ptr -> group_coord.x; //new global coords
    int y_coord_new = group_ptr -> group_coord.y;

    unsigned int x_size = group_ptr -> x_group_size - 2; //size of new block (without borders)
    unsigned int y_size = group_ptr -> y_group_size - 2;

    unsigned int x_start = 1; //local coords in old block that show start of useful data
    unsigned int y_start = 1;

    if (row_cells(group_ptr, 0)){ //if alive cells on lower group boundary expand downwards
        y_coord_new -= 1;
        y_size += 1;
        y_start -= 1;
    } else {
        for(unsigned int i = 1; (!row_cells(group_ptr, i)) && (y_size != 0); i++){ //cut empty rows from the bottom
            y_coord_new += 1;
            y_size -= 1;
            y_start += 1;
        }
    }


    if (row_cells(group_ptr, group_ptr -> y_group_size - 1)){ //if alive cells on upper group boundary expand upwards
        y_size += 1;
    } else {
        for(unsigned int i = 1; (!row_cells(group_ptr, group_ptr -> y_group_size - 1 - i)) && (y_size != 0); i++){ //cut empty rows from the top
            y_size -= 1;
        }
    }


    if (column_cells(group_ptr, 0)){ //if alive cells on left group boundary expand left
        x_coord_new -= 1;
        x_size += 1;
        x_start -= 1;
    } else {
        for(unsigned int i = 1; (!column_cells(group_ptr, i)) && (x_size != 0); i++){ //cut empty rows from the left
            x_coord_new += 1;
            x_size -= 1;
            x_start += 1;
        }
    }


    if (column_cells(group_ptr, group_ptr -> x_group_size - 1)){ //if alive cells on right group boundary expand right
        x_size += 1;
    } else {
        for(unsigned int i = 1; (!column_cells(group_ptr, group_ptr -> x_group_size - 1 - i)) && (x_size != 0); i++){ //cut empty rows from the right
            x_size -= 1;
        }
    }

    int x_offset = (x_coord_new - (group_ptr -> group_coord.x));
    int y_offset = (y_coord_new - (group_ptr -> group_coord.y));

    if ((x_offset == 0) && (y_offset == 0) && //no need to realloc if field size has not changed
         (x_size == group_ptr -> x_group_size - 2) &&
         (y_size == group_ptr -> y_group_size - 2)){
        return 0;
    }

    if ((x_size == 0) || (y_size == 0)) 
        return 1;

    void* new_block = calloc((x_size + 2) * (y_size + 2), 1);

    //printf("xsize = %d; ysize = %d\n", x_size, y_size);
    //printf("xstart = %d; ystart = %d\n", x_start, y_start);

    for(unsigned int i = 0; i < x_size; i++){
        for(unsigned int j = 0; j < y_size; j++){
            //printf("[%d][%d]\n", i, j);
            COORDVAL(new_block, x_size + 2, i + 1, j + 1) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x_start + i, y_start + j);
        }
    }

    free(group_ptr -> group_block);
    group_ptr -> group_block = new_block;
    group_ptr -> x_group_size = x_size + 2;
    group_ptr -> y_group_size = y_size + 2;
    group_ptr -> group_coord.x = x_coord_new;
    group_ptr -> group_coord.y = y_coord_new;

    return 0;
}

field_node* group_split(Field* field_ptr, field_node* node_ptr, unsigned int x, unsigned int y){
    Group* group_ptr = node_ptr -> group_ptr;
    field_node* first_node = (field_node*) malloc(sizeof(field_node));
    field_node* second_node = (field_node*) malloc(sizeof(field_node));
        
    //set up all the addresses

    if (node_ptr -> prev != NULL)
        node_ptr -> prev -> next = first_node;

    if (node_ptr -> next != NULL)
        node_ptr -> next -> prev = second_node;

    first_node -> prev = node_ptr -> prev;
    first_node -> next = second_node;

    second_node -> prev = first_node;
    second_node -> next = node_ptr -> next;

    first_node -> group_ptr = (Group*) malloc(sizeof(Group));
    second_node -> group_ptr = (Group*) malloc(sizeof(Group));

    //distribute block
    
    Group* first_group = first_node -> group_ptr;
    Group* second_group = second_node -> group_ptr;

    first_group -> x_group_size = (x != 0) ? (x) : (group_ptr -> x_group_size);
    first_group -> y_group_size = (y != 0) ? (y) : (group_ptr -> y_group_size);
    first_group -> group_block = calloc(first_group -> x_group_size * first_group -> y_group_size, 1);

    first_group -> group_coord.x = group_ptr -> group_coord.x;
    first_group -> group_coord.y = group_ptr -> group_coord.y;

    second_group -> x_group_size = (x != 0) ? (group_ptr -> x_group_size - x - 1) : (group_ptr -> x_group_size);
    second_group -> y_group_size = (y != 0) ? (group_ptr -> y_group_size - y - 1) : (group_ptr -> y_group_size);
    second_group -> group_block = calloc(second_group -> x_group_size * second_group -> y_group_size, 1);

    second_group -> group_coord.x = group_ptr -> group_coord.x;
    if (x != 0)
        second_group -> group_coord.x += x + 1;
    second_group -> group_coord.y = group_ptr -> group_coord.y;
    if (y != 0)
        second_group -> group_coord.y += y + 1;

    if (y == 0) { //split on a column
        for (unsigned int i = 0; i < x; i++) {
            for (unsigned int j = 0; j < group_ptr -> y_group_size; j++) {
                COORDVAL(first_group -> group_block, first_group -> x_group_size, i, j) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, j);
            }
        }

        for (unsigned int i = x + 1; i < group_ptr -> x_group_size; i++) {
            for (unsigned int j = 0; j < group_ptr -> y_group_size; j++) {
                COORDVAL(second_group -> group_block, second_group -> x_group_size, i - x - 1, j) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, j);
            }
        }
        
        free(group_ptr -> group_block);
        free(group_ptr);
        free(node_ptr);

        group_resize(first_group);
        group_resize(second_group);

        while (first_node -> prev != NULL)
            first_node = first_node -> prev;

        field_ptr[0] = first_node;

        return first_node;

    }

    if (x == 0) { //split on a row
        for (unsigned int j = 0; j < y; j++) {
            for (unsigned int i = 0; i < group_ptr -> x_group_size; i++) {
                COORDVAL(first_group -> group_block, first_group -> x_group_size, i, j) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, j);
            }
        }

        for (unsigned int j = y + 1; j < group_ptr -> y_group_size; j++) {
            for (unsigned int i = 0; i < group_ptr -> x_group_size; i++) {
                COORDVAL(second_group -> group_block, second_group -> x_group_size, i, j - y - 1) = COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, j);
            }
        }

        
        free(group_ptr -> group_block);
        free(group_ptr);
        free(node_ptr);

        group_resize(first_group);
        group_resize(second_group);

        while (first_node -> prev != NULL)
            first_node = first_node -> prev;

        field_ptr[0] = first_node;

        return first_node;

    }

    return node_ptr;
}

//------------Counters-------------------------

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y){
    uint8_t count = 0;

	if (x != group_ptr -> x_group_size - 1) {
        for(char i = ((y == 0) ? (0) : (-1)); i <= ((y == group_ptr -> y_group_size - 1) ? (0) : (1)); i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x + 1, y + i);
        }
    }

    if (x != 0) {
        for(char i = ((y == 0) ? (0) : (-1)); i <= ((y == group_ptr -> y_group_size - 1) ? (0) : (1)); i++) {
            count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x - 1, y + i);
        }
    }

    if (y != group_ptr -> y_group_size - 1) {
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y + 1);
    }

    if (y != 0) {
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y - 1);
    }

    return count;
}

int row_cells(Group* group_ptr, unsigned int y){
    unsigned int count = 0;
    for(unsigned int i = 0; i < group_ptr -> x_group_size; i++){
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, i, y);
    }
    return count;
}

int column_cells(Group* group_ptr, unsigned int x){
    unsigned int count = 0;
    for(unsigned int i = 0; i < group_ptr -> y_group_size; i++){
        count += COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, i);
    }
    return count;
}

//------------Global Cell Adressing------------

Group* find_cell_group(Field* field_ptr, int x, int y){

    field_node* cur_node = field_ptr[0];
    Group* target_group = NULL;

    for(int i = 0; cur_node != NULL; i++){
        target_group = cur_node -> group_ptr;
        if ((x < target_group -> group_coord.x) || 
            (y < target_group -> group_coord.y) ||
            (x > (target_group -> group_coord.x + (int) target_group -> x_group_size)) ||
            (y > (target_group -> group_coord.y + (int) target_group -> y_group_size))) {

            cur_node = cur_node -> next;
            continue;
        }

        return target_group;

    }

    return NULL;
}

uint8_t global_cell_status(Field* field_ptr, int x, int y){
    Group* target_group = find_cell_group(field_ptr, x, y);
    return COORDVAL(target_group -> group_block, target_group -> x_group_size, x - target_group -> group_coord.x, y - target_group -> group_coord.y);
}

int global_neighbour_count(Field* field_ptr, int x, int y){
    if ((x == INT_MAX) || (x == INT_MIN) || (y == INT_MAX) || (y == INT_MIN)) return 0;
    
    uint8_t count = 0;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++){
           count += global_cell_status(field_ptr, x + i, y + j); 
        }
    }

    return (count - global_cell_status(field_ptr, x, y)); //this call of global_cell_status could be removed if needed
}

//------------Field split/merge---------------

int check_common_borders(Field* field_ptr, Group* cur_group, Group* other_group){
    //when borders are just touching it is fine, all groups are already resized, no need to merge
    if ((other_group -> group_coord.x >= (cur_group -> group_coord.x + (int) cur_group -> x_group_size)) || //other is too far right
        (other_group -> group_coord.y >= (cur_group -> group_coord.y + (int) cur_group -> y_group_size)) || //other is too far up
        (cur_group -> group_coord.x >= (other_group -> group_coord.x + (int) other_group -> x_group_size)) || //cur is too far right
        (cur_group -> group_coord.y >= (other_group -> group_coord.y + (int) other_group -> y_group_size))) { //cur is too far up
        return 0;   
    }

    //if groups are intersecting by 1 or mode colums/rows - merge
    if ((other_group -> group_coord.x <= (cur_group -> group_coord.x + (int) cur_group -> x_group_size - 1)) || //the opposite to previous checks
        (other_group -> group_coord.y <= (cur_group -> group_coord.y + (int) cur_group -> y_group_size - 1)) || 
        (cur_group -> group_coord.x <= (other_group -> group_coord.x + (int) other_group -> x_group_size - 1)) || 
        (cur_group -> group_coord.y <= (other_group -> group_coord.y + (int) other_group -> y_group_size - 1))) { 
        return 1;   
    }

    return 0;
}

field_node* check_intersections(Field* field_ptr, field_node* cur_node){
    field_node* iter_node = field_ptr[0];

    while (iter_node != NULL) {
        if ((iter_node != cur_node) && check_common_borders(field_ptr, cur_node -> group_ptr, iter_node -> group_ptr))
            return iter_node;
        iter_node = iter_node -> next;
    }

    return NULL;
}

field_node* group_merge(Field* field_ptr, field_node* cur_node, field_node* other_node){
    Group* cur_group = cur_node -> group_ptr;
    Group* other_group = other_node -> group_ptr;

    coord_pair new_coord; //bottom left 
    unsigned int x_size;
    unsigned int y_size;

    new_coord.x = MIN(cur_group -> group_coord.x, other_group -> group_coord.x);
    new_coord.y = MIN(cur_group -> group_coord.y, other_group -> group_coord.y);

    x_size = MAX(cur_group -> group_coord.x + cur_group -> x_group_size - 1, other_group -> group_coord.x + other_group -> x_group_size - 1) - new_coord.x + 1;
    y_size = MAX(cur_group -> group_coord.y + cur_group -> y_group_size - 1, other_group -> group_coord.y + other_group -> y_group_size - 1) - new_coord.y + 1;

    //printf("xsize = %d; ysize = %d\n", x_size, y_size);

    Group* new_group = (Group*) malloc(sizeof(Group));

    field_node* new_node = (field_node*) malloc(sizeof(field_node));
    new_node -> prev = cur_node -> prev;//put new node in place of cur_node
    new_node -> next = cur_node -> next;
    new_node -> group_ptr = new_group;

    if (cur_node -> prev != NULL) //remove cur_node from the list
        cur_node -> prev -> next = new_node;
    
    if (cur_node -> next != NULL)
        cur_node -> next -> prev = new_node;

    if (other_node -> prev != NULL) //remove other_node from the list
        other_node -> prev -> next = other_node -> next;
    
    if (other_node -> next != NULL)
        other_node -> next -> prev = other_node -> prev;


    new_group -> group_coord = new_coord;
    new_group -> x_group_size = x_size;
    new_group -> y_group_size = y_size;
    new_group -> group_block = calloc(x_size * y_size, 1);

    for (unsigned int x = 0; x < (cur_group -> x_group_size); x++) {
        for (unsigned int y = 0; y < (cur_group -> y_group_size); y++) {
            COORDVAL(new_group -> group_block, x_size, x + (unsigned int) (cur_group -> group_coord.x - new_coord.x), y + (unsigned int) (cur_group -> group_coord.y - new_coord.y)) = 
                COORDVAL(cur_group -> group_block, cur_group -> x_group_size, x, y);
        }
    }

    for (unsigned int x = 0; x < (other_group -> x_group_size); x++) {
        for (unsigned int y = 0; y < (other_group -> y_group_size); y++) {
            COORDVAL(new_group -> group_block, x_size, x + (unsigned int) (other_group -> group_coord.x - new_coord.x), y + (unsigned int) (other_group -> group_coord.y - new_coord.y)) = 
                COORDVAL(other_group -> group_block, other_group -> x_group_size, x, y);
        }
    }

    free(cur_group -> group_block);
    free(cur_group);
    free(cur_node);

    free(other_group -> group_block);
    free(other_group);
    free(other_node);

    cur_node = new_node;
    while (new_node -> prev != NULL)
        new_node = new_node -> prev;

    field_ptr[0] = new_node;

    return cur_node;
}

int field_merge(Field* field_ptr){
    field_node* cur_node = field_ptr[0];
      
    while (cur_node != NULL) {

        while (1) {
            field_node* other_node = check_intersections(field_ptr, cur_node); 
            if (other_node == NULL)
                break;
            cur_node = group_merge(field_ptr, cur_node, other_node);
        }

        cur_node = cur_node -> next;
    }

    return 0;
}

int field_split(Field* field_ptr){ //might be place for optimizations (in cur_node assignment)

    if ((field_ptr == NULL) || (field_ptr[0] == NULL)) 
        return -1;

    field_node* cur_node = field_ptr[0];
    
    while (1) {
        
        Group* cur_group = cur_node -> group_ptr;

        for (unsigned int x = 1; x < cur_group -> x_group_size - 2; x++) {


            if ((column_cells(cur_group, x) == 0) && (column_cells(cur_group, x + 1) == 0)) { //check for an empty columns
                cur_node = group_split(field_ptr, cur_node, x, 0);
                //printf("split on x = %d\n", x);
            }

            cur_group = cur_node -> group_ptr;

        }

        for (unsigned int y = 1; y < cur_group -> y_group_size - 2; y++) {

            if ((row_cells(cur_group, y) == 0) && (row_cells(cur_group, y + 1) == 0)) { //check for an empty rows

                cur_node = group_split(field_ptr, cur_node, 0, y);
                //printf("split on y = %d\n", y);
            }

            cur_group = cur_node -> group_ptr;

        }

        if (cur_node -> next == NULL)
            break;

        cur_node = cur_node -> next;
    }

    while (cur_node -> prev != NULL){
        cur_node = cur_node -> prev;
    }
    
    field_ptr[0] = cur_node;

    return 0;
}

#undef COORDVAL
#undef MAX
#undef MIN
