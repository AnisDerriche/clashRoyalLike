#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"
#include "utils.h"
using namespace std;


internal
void simulate_menu(float dt, State& currentState) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
		currentState = State::IN_GAME;
		cout << "Lancement du jeu !" << endl;
		cout << dt;
	}
}