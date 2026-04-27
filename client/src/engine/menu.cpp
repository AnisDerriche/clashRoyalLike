#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"
#include "utils.h"
using namespace std;

void draw_menu(sf::RenderWindow& window, Side side) {
	sf::RectangleShape startButton(sf::Vector2f(200, 50));
	startButton.setPosition(sf::Vector2f(300, 300));
	startButton.setFillColor(sf::Color::White);
	window.draw(startButton);

	// Indicateur de côté
	sf::RectangleShape sideIndicator(sf::Vector2f(50, 50));
	sideIndicator.setPosition(sf::Vector2f(375, 400));
	sideIndicator.setFillColor(side == Side::PLAYER ? sf::Color::Blue : sf::Color::Red);
	window.draw(sideIndicator);

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
