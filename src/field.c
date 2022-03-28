#include "field.h"
#include <stdlib.h>
#include <limits.h>

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + SX * (Y) + X))

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y);
int row_cells(Group* group_ptr, unsigned int y);
int column_cells(Group* group_ptr, unsigned int x);
int group_resize(Group* group_ptr);
field_node* group_split(field_node* node_ptr, unsigned int x, unsigned int y); 
Group* find_cell_group(Field* field_ptr, int x, int y);
uint8_t global_cell_status(Field* field_ptr, int x, int y);
int global_neighbour_count(Field* field_ptr, int x, int y);
int check_common_borders(Field* field_ptr, Group* cur_group, Group* other_group);
int group_merge(field_node* cur_node, field_node* other_node);
field_node* check_intersections(Field* field_ptr, field_node* cur_node);

void field_free(Field* field_ptr){
        
    field_node* cur_node = field_ptr[0];

    for(int i = 0; cur_node -> next != NULL; i++){
        free(cur_node -> group_ptr -> group_block);
        cur_node = cur_node -> next;
        free(cur_node -> prev);    
    }

    free(cur_node -> group_ptr -> group_block);
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

int field_step(Field* field_ptr){

    field_node* cur_node = field_ptr[0];

    for(int i = 0; cur_node != NULL; i++){
        group_step(cur_node -> group_ptr);
        cur_node = cur_node -> next;
    }

    field_resplit(field_ptr);

    return 0;
}

int group_resize(Group* group_ptr){
    int x_coord_start = group_ptr -> group_coord.x;
    int y_coord_start = group_ptr -> group_coord.y;
    unsigned int x_start = 0;
    unsigned int y_start = 0;
    unsigned int x_end = group_ptr -> x_group_size;
    unsigned int y_end = group_ptr -> y_group_size;
    

    if (row_cells(group_ptr, 0)){ //if alive cells on lower group boundary expand downwards
        group_ptr -> group_coord.y -= 1;
        group_ptr -> y_group_size += 1;
    }

    if (row_cells(group_ptr, y_end)){ //if alive cells on upper group boundary expand upwards
        group_ptr -> y_group_size += 1;
    }

    if (column_cells(group_ptr, 0)){ //if alive cells on left group boundary expand left
        group_ptr -> group_coord.x -= 1;
        group_ptr -> x_group_size += 1;
    }

    if (column_cells(group_ptr, x_end)){ //if alive cells on right group boundary expand right
        group_ptr -> x_group_size += 1;
    }


    for(unsigned int i = 1; !row_cells(group_ptr, i); i++){ //cut empty rows from the bottom
        group_ptr -> group_coord.y += 1;
        group_ptr -> y_group_size -= 1;
        y_start += 1;
    }

    for(unsigned int i = 1; !row_cells(group_ptr, y_end - i); i++){ //cut empty rows from the top
        group_ptr -> y_group_size -= 1;
        y_end -= 1;
    }

    for(unsigned int i = 1; !column_cells(group_ptr, i); i++){ //cut empty rows from the left
        group_ptr -> group_coord.x += 1;
        group_ptr -> x_group_size -= 1;
        x_start += 1;
    }

    for(unsigned int i = 1; !column_cells(group_ptr, x_end - i); i++){ //cut empty rows from the right
        group_ptr -> x_group_size -= 1;
        x_end -= 1;
    }

    unsigned int x_offset = (unsigned int) (x_coord_start - (group_ptr -> group_coord.x));
    unsigned int y_offset = (unsigned int) (y_coord_start - (group_ptr -> group_coord.y));

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

field_node* group_split(field_node* node_ptr, unsigned int x, unsigned int y){
    Group* group_ptr = node_ptr -> group_ptr;
    field_node* first_node = malloc(sizeof(field_node));
    field_node* second_node = malloc(sizeof(field_node));
        
    //set up all the addresses
    node_ptr -> prev -> next = first_node;
    node_ptr -> next -> prev = second_node;

    first_node -> prev = node_ptr -> prev;
    first_node -> next = second_node;

    second_node -> prev = first_node;
    second_node -> next = node_ptr -> next;

    //distribute block
    
    Group* first_group = first_node -> group_ptr;
    Group* second_group = second_node -> group_ptr;

    first_group -> x_group_size = (x) ? (x) : (group_ptr -> x_group_size);
    first_group -> y_group_size = (y) ? (y) : (group_ptr -> y_group_size);
    first_group -> group_block = malloc(first_group -> x_group_size * first_group -> y_group_size);

    second_group -> x_group_size = (x) ? (group_ptr -> x_group_size - x - 1) : (group_ptr -> x_group_size);
    second_group -> y_group_size = (y) ? (group_ptr -> y_group_size - y - 1) : (group_ptr -> y_group_size);
    second_group -> group_block = malloc(second_group -> x_group_size * second_group -> y_group_size);

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

        return first_node;

    }

    return node_ptr;
}

//------------Counters-------------------------

int neighbour_count(Group* group_ptr, unsigned int x, unsigned int y){
    uint8_t count = 0;

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
            (y > (target_group -> group_coord.y + (int) target_group -> y_group_size)))
            continue;

        return target_group;

        cur_node = cur_node -> next;
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

int field_merge(Field* field_ptr){
      field_node* cur_node = field_ptr[0];
      
      while (cur_node != NULL) {
          Group* cur_group = cur_node -> group_ptr;  
          field_node* other_node = cur_node -> next;
          
          while (other_node != NULL) {
              Group* other_group = other_node -> group_ptr;
              
              if (check_common_borders(field_ptr, cur_group, other_group)){ //checks by coord first; by cell activity if needed
                  group_merge(cur_node, other_node);
                  while (1) {
                      other_node = check_intersections(field_ptr, cur_node);
                      if (other_group == NULL)
                          break;
                      group_merge(cur_node, other_node);
                  }                
              }

              other_node = other_node -> next;
          }

          cur_node = cur_node -> next;
      }
      
      return 0;
}

int field_split(Field* field_ptr){ //might be place for optimizations (in cur_node assignment)

    field_node* cur_node = field_ptr[0];
    
    while (cur_node != NULL) {
        
        Group* cur_group = cur_node -> group_ptr;

        for (unsigned int x = 0; x < cur_group -> x_group_size; x++) {

            if (column_cells(cur_group, x) != 0) { //check for an empty column
                uint8_t flag = 0;
                for (unsigned int y = 0; y < cur_group -> y_group_size; y++) { //if empty, check if any cell will be born
                    if (neighbour_count(cur_group, x, y) == 3){ //if there are such cells...
                        flag = 1;
                        break;
                    }
                }
                if (flag) 
                    continue; //...skip the column
            }

            cur_node = group_split(cur_node, x, 0);

        }

        for (unsigned int y = 0; y < cur_group -> y_group_size; y++) {

            if (row_cells(cur_group, y) != 0) { //check for an empty row
                uint8_t flag = 0;
                for (unsigned int x = 0; x < cur_group -> x_group_size; x++) { //if empty, check if any cell will be born
                    if (neighbour_count(cur_group, x, y) == 3){ //if there are such cells...
                        flag = 1;
                        break;
                    }
                }
                if (flag) 
                    continue; //...skip the row
            }

            cur_node = group_split(cur_node, 0, y);

        }

        cur_node = cur_node -> next;
    }
    
    return 0;
}

#undef COORDVAL
