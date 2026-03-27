#include <SFML/Graphics.hpp>
#include <iostream>
#include "states.h"
#include "menu.h"
using namespace std;

int main() {
	// Mode initial : Fenêtré
	sf::VideoMode windowMode({ 800, 600 });
	sf::RenderWindow window(windowMode, "Royal Arena Client");
	bool isFullscreen = true;

	sf::Clock clock;
	State currentState = State::MAIN_MENU;

	float delta_time = 0.16666f;

	if (isFullscreen) {
		// Passage en plein écran
		window.create(sf::VideoMode::getDesktopMode(), "Royal Arena", sf::State::Fullscreen);
	}

	while (window.isOpen()) {
		// Nouvelle syntaxe SFML 3
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}


			float delta_time = clock.restart().asSeconds();

			// Détection de touches
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

			}

			switch (currentState) {
			case State::MAIN_MENU:
				simulate_menu(delta_time, currentState);
				break;

			case State::IN_GAME:
				break;
			}


		}
		window.clear(sf::Color(100, 149, 237));

		/*		// On choisit quoi dessiner en fonction de l'état
				if (currentState == State::MAIN_MENU) {
					// Ici tu appelleras tes fonctions de dessin du menu
					// ex: draw_menu(window);
				}
				else if (currentState == State::IN_GAME) {
					// ex: draw_game(window);
				}*/

		window.display();
	}
	return 0;
}