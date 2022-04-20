#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "field.h"

int draw_group(sf::RenderWindow &window, Group* group_ptr, int x_offset, int y_offset);
int draw_field(sf::RenderWindow &window, Field* field_ptr, int x_offset, int y_offset);
int draw_cell(sf::RenderWindow &window, int x, int y);
void group_dump(Group* group_ptr);
void field_dump(Field* field_ptr);
