#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "field.h"
#include "draw.h"

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

    // big oscillator
    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 4 + i, 4) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, i) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, 6 + i) = 1;
    }

    /* two oscillators
    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i, 1) = 1;
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, i + 4, 1) = 1;
    }
    */

    /*
    for (int i = 0; i < 3; i++) {
        COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 2, 5 + i) = 1;
    }
    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 1, 2) = 1;
    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 2, 1) = 1;
    COORDVAL(field -> group_ptr -> group_block, field -> group_ptr -> x_group_size, 3, 2) = 1;
    */

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
                field_step(&field);
            }
        }

        if ((sf::Mouse::isButtonPressed(sf::Mouse::Left) && (clock.getElapsedTime().asMilliseconds() - time > 300)) ||
            (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (clock.getElapsedTime().asMilliseconds() - time > 100))) {
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
