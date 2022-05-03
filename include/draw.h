#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "field.h"
#include "analysis.h"

#define CELL_SIZE 10

int draw_group(sf::RenderWindow &window, Group* group_ptr, int x_offset, int y_offset);
int draw_field(sf::RenderWindow &window, Field* field_ptr, int x_offset, int y_offset);
int draw_cell(sf::RenderWindow &window, int x, int y);
int color_status(sf::RenderWindow &window, History* hist, int x_offset, int y_offset);
void group_dump(Group* group_ptr);
void field_dump(Field* field_ptr);
