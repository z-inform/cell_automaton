#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "field.h"
#include "draw.h"
#include "generator.h"

#define COORDVAL(A, SX, X, Y) (*((uint8_t*)A + (SX) * (Y) + X))

#define XSIZE 10u
#define YSIZE 10u

void group_dump(Group* group_ptr);
void dumb_dump(Group* group_ptr);
void field_dump(Field* field_ptr);

int main(){

    field_node* field = (field_node*) malloc(sizeof(field_node));
    field -> prev = NULL;
    field -> next = NULL;
    field -> group_ptr = (Group*) malloc(sizeof(Group));
    field -> group_ptr -> group_coord.x = 0;
    field -> group_ptr -> group_coord.y = 0;
    field -> group_ptr -> x_group_size = XSIZE;
    field -> group_ptr -> y_group_size = YSIZE;
    field -> group_ptr -> group_block = calloc(XSIZE * YSIZE, 1);

    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 4 + i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, i) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, 6 + i) = 1;
    }


    group_resize(field -> group_ptr);


    sf::RenderWindow window(sf::VideoMode(1500, 700), "OAOA MMMM", sf::Style::Titlebar | sf::Style::Close);
    sf::Clock clock;
    int64_t time = clock.getElapsedTime().asMilliseconds();

    while (window.isOpen()) {
        sf::Event event;


        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                coord_pair coord; //coord of cell under cursor
                coord.x = (mouse_pos.x - window.getSize().x / 2) / 12;
                coord.y = (mouse_pos.y - window.getSize().y / 2) / 12;
                Group* group_ptr = find_cell_group(&field, coord.x, coord.y);
                if (group_ptr == NULL) { //check if group with this cell exists
                    group_ptr = (Group*) malloc(sizeof(Group)); //create a group if not
                    group_ptr -> group_coord.x = coord.x - 1;
                    group_ptr -> group_coord.y = coord.y - 1;
                    group_ptr -> x_group_size = 3;
                    group_ptr -> y_group_size = 3;
                    group_ptr -> group_block = calloc(9, 1);
                    COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, 1, 1) = 1;
                    add_group(&field, group_ptr);
                    field_merge(&field);
                    field_split(&field);
                } else { //if exists just toggle the cell
                    COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, coord.x - group_ptr -> group_coord.x, coord.y - group_ptr -> group_coord.y) =
                        !COORDVAL(group_ptr -> group_block, group_ptr -> x_group_size, coord.x - group_ptr -> group_coord.x, coord.y - group_ptr -> group_coord.y);
                    if (group_resize(group_ptr) == 1)
                        remove_group(&field, group_ptr);
                    field_merge(&field);
                    field_split(&field);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case (sf::Keyboard::Enter):
                        break;

                    case (sf::Keyboard::Q):
                        window.close();
                        break;
             
                    case (sf::Keyboard::C):
                        field_free(&field);
                        break;
                }
            }

        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && (clock.getElapsedTime().asMilliseconds() - time > 300)) {
            field_step(&field);
            time = clock.getElapsedTime().asMilliseconds();
        }


        window.clear();
        draw_field(window, &field);
        window.display();

    }

    field_free(&field);
    return 0;
}

void dumb_dump(Group* group_ptr){
    for (unsigned int i = 0; i < (group_ptr -> x_group_size) * (group_ptr -> y_group_size); i++){
        printf("%hhu ", ((uint8_t*)group_ptr -> group_block)[i]);
    }
    printf("\n");
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
