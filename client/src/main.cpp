#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <cstdint>
#include "states.h"
#include "menu.h"
#include "game_logic.h"
#include "game.pb.h"

using namespace std;

int main() {
	// 1. Paramètres de la fenêtre
	const sf::Vector2u windowSize(800, 1000);
	
	// Mode par défaut (avec barre de titre pour pouvoir la déplacer)
	sf::RenderWindow window(sf::VideoMode(windowSize), "Royal Arena Client", sf::Style::Default);
	window.setFramerateLimit(60);

	// 2. Centrer la fenêtre sur l'écran
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	window.setPosition(sf::Vector2i(
		(desktop.size.x - windowSize.x) / 2,
		(desktop.size.y - windowSize.y) / 2
	));

	sf::Clock clock;
	State currentState = State::MAIN_MENU;
	Battle battle(true); // true = isClient

	// Résolution virtuelle fixe
	sf::View gameView(sf::FloatRect({0, 0}, {800, 1000}));
	
	// Network
	sf::UdpSocket socket;
	socket.setBlocking(false);
	
	// Lecture de l'IP du serveur depuis un fichier ou fallback en local
	std::string ipString = "127.0.0.1";
	std::ifstream ipFile("server_ip.txt");
	if (ipFile.is_open()) {
		std::getline(ipFile, ipString);
		ipFile.close();
		cout << "IP du serveur chargee depuis server_ip.txt : " << ipString << endl;
	} else {
		cout << "Fichier server_ip.txt introuvable, utilisation de localhost : " << ipString << endl;
	}

	auto serverIp = sf::IpAddress::resolve(ipString);
	std::uint16_t serverPort = 50000;

	if (serverIp) {
		battle.setNetwork(&socket, *serverIp, serverPort);
		sf::Packet ping;
		ping << "ping";
		socket.send(ping, *serverIp, serverPort);
	}

	while (window.isOpen()) {
		float delta_time = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}

			// On retire la gestion du Resized pour empêcher tout changement de dimension
			
			if (currentState == State::IN_GAME) {
				battle.handleInput(window, *event, gameView);
			}

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				// Touche Escape pour quitter (nécessaire car plus de bouton fermer)
				if (keyPressed->code == sf::Keyboard::Key::Escape) {
					window.close();
				}
				if (currentState == State::MAIN_MENU) {
					if (keyPressed->code == sf::Keyboard::Key::Enter) {
						currentState = State::IN_GAME;
					}
					if (keyPressed->code == sf::Keyboard::Key::S) {
						if (battle.getClientSide() == Side::PLAYER) {
							battle.setClientSide(Side::ENEMY);
							gameView.setRotation(sf::Degrees(180.f));
						} else {
							battle.setClientSide(Side::PLAYER);
							gameView.setRotation(sf::Degrees(0.f));
						}
						cout << "Side toggled to: " << (battle.getClientSide() == Side::PLAYER ? "PLAYER" : "ENEMY") << endl;
					}
				}
			}
		}

		if (currentState == State::IN_GAME) {
			sf::Packet packet;
			std::optional<sf::IpAddress> remoteIp;
			unsigned short remotePort = 0;
			while (socket.receive(packet, remoteIp, remotePort) == sf::Socket::Status::Done) {
				std::string data;
				packet >> data;
				battle.deserialize(data);
			}
		}

		window.clear(sf::Color(100, 149, 237));

		if (currentState == State::MAIN_MENU) {
			simulate_menu(delta_time, currentState);
			draw_menu(window, battle.getClientSide());
		}
		else if (currentState == State::IN_GAME) {
			window.setView(gameView);
			battle.draw(window);
		}

		window.display();
	}
	return 0;
}
