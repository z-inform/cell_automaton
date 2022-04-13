#include <SFML/Graphics.hpp>

#include "field.h"
#include "draw.h"

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + (SX) * (Y) + X))

int draw_cell(sf::RenderWindow &window, int x, int y){
    sf::RectangleShape cell_rect;
    cell_rect.setSize(sf::Vector2f(12, 12));
    cell_rect.setOutlineColor(sf::Color(0, 0, 0));
    cell_rect.setOutlineThickness(-0.5);
    //sf::Vector2u size = window.getSize();
    cell_rect.setPosition(x * 12, y * 12);
    window.draw(cell_rect);
    return 0;
}

int draw_group(sf::RenderWindow &window, Group* group_ptr){

    sf::RectangleShape outline;
    outline.setSize(sf::Vector2f(group_ptr -> x_group_size * 12, group_ptr -> y_group_size * 12));
    outline.setOutlineColor(sf::Color(128, 0, 0));
    outline.setOutlineThickness(1);
    outline.setFillColor(sf::Color::Transparent);
    outline.setPosition((group_ptr -> group_coord.x) * 12,(group_ptr -> group_coord.y) * 12);
    window.draw(outline);

    for (unsigned int x = 0; x < group_ptr -> x_group_size; x++) {
        for (unsigned int y = 0; y < group_ptr -> y_group_size; y++) {
            if (COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y))
                draw_cell(window, group_ptr -> group_coord.x + (int) x, group_ptr -> group_coord.y + (int) y);
        }
    }

    return 0;
}

int draw_field(sf::RenderWindow &window, Field* field_ptr){
    if ((field_ptr == NULL) || (field_ptr[0] == NULL))
        return -1;
    field_node* cur_node = field_ptr[0];

    while (cur_node != NULL) {
        draw_group(window, cur_node -> group_ptr);
        cur_node = cur_node -> next;
    }

    return 0;
}

void group_dump(Group* group_ptr){
    printf("global coord [%d][%d]\n", group_ptr -> group_coord.x, group_ptr -> group_coord.y);
    printf("size %d x %d\n", group_ptr -> x_group_size, group_ptr -> y_group_size);
    for (int y = (int) group_ptr -> y_group_size - 1; y >= 0;  y--) {
        for (unsigned int x = 0; x < group_ptr -> x_group_size; x++)
            printf("[%hhu]", COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y));
        printf("\n");
    }
}

void field_dump(Field* field_ptr){
    field_node* cur_node = field_ptr[0];
    while (cur_node != NULL) {
        
        group_dump(cur_node -> group_ptr);
        printf("\n");

        cur_node = cur_node -> next;
    }
}

