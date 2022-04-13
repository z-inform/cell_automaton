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

void dumb_dump(Group* group_ptr);

int main(){

    field_node* field = (field_node*) malloc(sizeof(field_node));
    field -> prev = NULL;
    field -> next = NULL;
    field -> group_ptr = (Group*) malloc(sizeof(Group));
    field -> group_ptr -> group_coord.x = 60;
    field -> group_ptr -> group_coord.y = 20;
    field -> group_ptr -> x_group_size = XSIZE;
    field -> group_ptr -> y_group_size = YSIZE;
    field -> group_ptr -> group_block = calloc(XSIZE * YSIZE, 1);

    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 4 + i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, i) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, 6 + i) = 1;
    }
    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 0, 0) = 1;


    group_resize(field -> group_ptr);
    //printf("base group size %ux%u\n", field -> group_ptr -> x_group_size, field -> group_ptr -> y_group_size);


    sf::RenderWindow window(sf::VideoMode(1500, 700), "OAOA MMMM", sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    sf::Clock clock;
    int64_t time = clock.getElapsedTime().asMilliseconds();
    bool auto_run = false;

    while (window.isOpen()) {
        sf::Event event;


        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                printf("Click on raw %d %d\n", mouse_pos.x, mouse_pos.y);
                coord_pair coord; //coord of cell under cursor
                coord.x = (mouse_pos.x) / 12;
                coord.y = (mouse_pos.y) / 12;
                printf("Click on %d %d\n", coord.x, coord.y);
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
                    printf("Click in existing group\n");
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
                        auto_run = !auto_run;
                        break;

                    case (sf::Keyboard::Q):
                        window.close();
                        break;
             
                    case (sf::Keyboard::C):
                        field_free(&field);
                        break;

                    default:
                        break;
                }
            }

        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || auto_run) && (clock.getElapsedTime().asMilliseconds() - time > 10)) {
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
