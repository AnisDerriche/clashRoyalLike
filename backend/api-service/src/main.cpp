#include <drogon/drogon.h>
#include <iostream>

int main() {
	std::cout << "Serveur API Drogon en cours de lancement sur le port 8080..." << std::endl;

	// Configuration simple
	drogon::app().addListener("0.0.0.0", 8080);

	// On lance le serveur (bloquant)
	drogon::app().run();

	return 0;
}