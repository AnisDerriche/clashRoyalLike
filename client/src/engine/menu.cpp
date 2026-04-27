#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"
#include "utils.h"
using namespace std;

void draw_menu(sf::RenderWindow& window) {
	sf::RectangleShape startButton(sf::Vector2f(200, 50));
	startButton.setPosition(sf::Vector2f(300, 300));
	startButton.setFillColor(sf::Color::White);
	window.draw(startButton);

	// Simple text-like representation
	sf::RectangleShape textRect(sf::Vector2f(10, 10));
	textRect.setPosition(sf::Vector2f(310, 320));
	textRect.setFillColor(sf::Color::Black);
	window.draw(textRect);
}

void simulate_menu(float dt, State& currentState) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
		currentState = State::IN_GAME;
		cout << "Lancement du jeu !" << endl;
	}
}
