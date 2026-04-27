#pragma once
#include <utils.h>
#include <states.h>
#include <game_logic.h>


void draw_menu(sf::RenderWindow& window, Side side);
void simulate_menu(float delta_time, State& currentState);