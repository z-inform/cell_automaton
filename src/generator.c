#include <stdlib.h>

#include "field.h"
#include "generator.h"

int add_group(Field* field_ptr, Group* group_ptr){
    if ((field_ptr == NULL) || (group_ptr == NULL))
        return -1;

    if (group_resize(group_ptr) == 1) //if the added group was empty do nothing
        return -2;

    field_node* node_ptr = (field_node*) malloc(sizeof(field_node));

    if (field_ptr[0] != NULL) {
        field_node* cur_node = field_ptr[0];
        while (cur_node -> next != NULL)
            cur_node = cur_node -> next;
        cur_node -> next = node_ptr;
        node_ptr -> prev = cur_node;
    } else {
        node_ptr -> prev = NULL;
        field_ptr[0] = node_ptr;
    }


    node_ptr -> next = NULL;
    node_ptr -> group_ptr = group_ptr;

    return 0;
}   

int remove_group(Field* field_ptr, Group* group_ptr) {
    field_node* cur_node = field_ptr[0];

    while ((cur_node != NULL) && (cur_node -> group_ptr != group_ptr)){
        cur_node = cur_node -> next;
    }

    if (cur_node == NULL)
        return -1;

    if (cur_node -> prev == NULL)
        field_ptr[0] = cur_node -> next;
    else
        cur_node -> prev -> next = cur_node -> next;

    if (cur_node -> next != NULL)
        cur_node -> next -> prev = cur_node -> prev;

    free(cur_node -> group_ptr -> group_block);
    free(cur_node -> group_ptr);
    free(cur_node);

    return 0;
}
