#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
	// SFML 3 utilise des objets sf::VideoMode pour la taille
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Royal Arena Client - SFML 3");

	std::cout << "Le client SFML 3 est lance avec succes !" << std::endl;

	while (window.isOpen()) {
		// Nouvelle syntaxe d'evenements SFML 3 (plus propre)
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}

		window.clear(sf::Color(100, 149, 237)); // Bleu Cornflower
		window.display();
	}

	return 0;
}