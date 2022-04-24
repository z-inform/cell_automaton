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

int draw_group(sf::RenderWindow &window, Group* group_ptr, int x_offset, int y_offset){

    sf::RectangleShape outline;
    outline.setSize(sf::Vector2f(group_ptr -> x_group_size * 12, group_ptr -> y_group_size * 12));
    outline.setOutlineColor(sf::Color(128, 0, 0));
    outline.setOutlineThickness(1);
    outline.setFillColor(sf::Color::Transparent);
    outline.setPosition((group_ptr -> group_coord.x + x_offset) * 12, (group_ptr -> group_coord.y + y_offset) * 12);
    window.draw(outline);

    for (unsigned int x = 0; x < group_ptr -> x_group_size; x++) {
        for (unsigned int y = 0; y < group_ptr -> y_group_size; y++) {
            if (COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, x, y))
                draw_cell(window, group_ptr -> group_coord.x + (int) x + x_offset, group_ptr -> group_coord.y + (int) y + y_offset);
        }
    }

    return 0;
}

int draw_field(sf::RenderWindow &window, Field* field_ptr, int x_offset, int y_offset){
    if ((field_ptr == NULL) || (field_ptr[0] == NULL))
        return -1;
    field_node* cur_node = field_ptr[0];

    while (cur_node != NULL) {
        draw_group(window, cur_node -> group_ptr, x_offset, y_offset);
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

int color_status(sf::RenderWindow &window, History* hist, int x_offset, int y_offset){

    if (hist[0] == NULL)
        return -1;

    state_node* cur_state = hist[0] -> state;

    while (cur_state != NULL) {
        Group* group_ptr = cur_state -> group_ptr;
        sf::RectangleShape color_block;
        color_block.setSize(sf::Vector2f(group_ptr -> x_group_size * 12, group_ptr -> y_group_size * 12));
        color_block.setPosition((group_ptr -> group_coord.x + x_offset) * 12, (group_ptr -> group_coord.y + y_offset) * 12);
        sf::Color color;
        switch (cur_state -> status) {
            case stable: //green for stable
                color.r = 51;
                color.g = 204; 
                color.b = 51;
                break;
            case oscillator: //orange for oscillator
                color.r = 255;
                color.g = 152; 
                color.b = 51;
                break;
            case glider: //blue for glider
                color.r = 0;
                color.g = 102; 
                color.b = 255;
                break;
            case unknown: //magenta for unknown
                color.r = 255;
                color.g = 0; 
                color.b = 255;
                break;
        }
        color.a = 50;
        color_block.setFillColor(color);
        window.draw(color_block);

        cur_state = cur_state -> next;
    }
    return 0;
}

