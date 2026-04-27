#include "game_logic.h"
#include "game.pb.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

uint32_t Entity::nextId = 0;

float distance(sf::Vector2f a, sf::Vector2f b) {
	return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

Unit::Unit(sf::Vector2f pos, float hp, float dmg, float rng, float spd, Side s, sf::Color c) {
	position = pos;
	health = hp;
	maxHealth = hp;
	damage = dmg;
	range = rng;
	speed = spd;
	side = s;
	color = c;
}

void Unit::update(float dt, std::vector<std::unique_ptr<Entity>>& entities) {
	if (health <= 0) {
		isDead = true;
		return;
	}

	if (!target || target->isDead) {
		findTarget(entities);
	}

	if (target) {
		float dist = distance(position, target->position);
		if (dist > range) {
			sf::Vector2f dir = target->position - position;
			float mag = std::sqrt(dir.x * dir.x + dir.y * dir.y);
			dir /= mag;
			position += dir * speed * dt;
		}
		else {
			target->health -= damage * dt;
		}
	}
}

void Unit::draw(sf::RenderWindow& window) {
	sf::RectangleShape shape(sf::Vector2f(20, 20));
	shape.setOrigin(sf::Vector2f(10, 10));
	shape.setPosition(position);
	shape.setFillColor(color);
	window.draw(shape);

	// Health bar
	sf::RectangleShape healthBar(sf::Vector2f(20 * (health / maxHealth), 5));
	healthBar.setPosition(sf::Vector2f(position.x - 10, position.y - 15));
	healthBar.setFillColor(sf::Color::Red);
	window.draw(healthBar);
}

void Unit::findTarget(std::vector<std::unique_ptr<Entity>>& entities) {
	float minDist = 1e9;
	Entity* closest = nullptr;
	for (auto& e : entities) {
		if (e->side != side && !e->isDead) {
			float dist = distance(position, e->position);
			if (dist < minDist) {
				minDist = dist;
				closest = e.get();
			}
		}
	}
	target = closest;
}

Tower::Tower(sf::Vector2f pos, float hp, float dmg, float rng, Side s) {
	position = pos;
	health = hp;
	maxHealth = hp;
	damage = dmg;
	range = rng;
	side = s;
	shape.setSize(sf::Vector2f(40, 40));
	shape.setOrigin(sf::Vector2f(20, 20));
	shape.setPosition(position);
	shape.setFillColor(s == Side::PLAYER ? sf::Color::Blue : sf::Color::Red);
}

void Tower::update(float dt, std::vector<std::unique_ptr<Entity>>& entities) {
	if (health <= 0) {
		isDead = true;
		return;
	}

	Entity* target = nullptr;
	float minDist = 1e9;
	for (auto& e : entities) {
		if (e->side != side && !e->isDead) {
			float dist = distance(position, e->position);
			if (dist < range && dist < minDist) {
				minDist = dist;
				target = e.get();
			}
		}
	}

	if (target) {
		target->health -= damage * dt;
	}
}

void Tower::draw(sf::RenderWindow& window) {
	if (isDead) return;
	window.draw(shape);

	// Health bar
	sf::RectangleShape healthBar(sf::Vector2f(40 * (health / maxHealth), 5));
	healthBar.setPosition(sf::Vector2f(position.x - 20, position.y - 30));
	healthBar.setFillColor(sf::Color::Green);
	window.draw(healthBar);
}

Battle::Battle(bool isClient) : isClient(isClient), serverIp(sf::IpAddress::Any) {
	playerElixir = 5.0f;
	enemyElixir = 5.0f;

	// Coordonnées symétriques autour du centre (y=400)
	// Tours Joueur (Bas)
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(400, 750), 2000, 50, 200, Side::PLAYER)); // Roi
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(250, 550), 1000, 40, 150, Side::PLAYER));
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(550, 550), 1000, 40, 150, Side::PLAYER));

	// Tours Ennemi (Haut)
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(400, 50), 2000, 50, 200, Side::ENEMY)); // Roi
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(250, 250), 1000, 40, 150, Side::ENEMY));
	entities.push_back(std::make_unique<Tower>(sf::Vector2f(550, 250), 1000, 40, 150, Side::ENEMY));

	playerHand.push_back({ "Knight", 3, sf::Color::Cyan });
	playerHand.push_back({ "Archer", 2, sf::Color::Green });
	playerHand.push_back({ "Giant", 5, sf::Color::Yellow });
}

void Battle::update(float dt) {
	if (gameOver) return;
	updateElixir(dt);

	bool playerKingDead = true;
	bool enemyKingDead = true;

	for (auto& e : entities) {
		e->update(dt, entities);

		Tower* t = dynamic_cast<Tower*>(e.get());
		if (t && !t->isDead) {
			// Le Roi est la tour centrale sur son axe respectif
			if (t->position.x == 400) {
				if (t->side == Side::PLAYER && t->position.y > 600) playerKingDead = false;
				if (t->side == Side::ENEMY && t->position.y < 200) enemyKingDead = false;
			}
		}
	}

	if (playerKingDead) {
		gameOver = true;
		winner = Side::ENEMY;
	}
	else if (enemyKingDead) {
		gameOver = true;
		winner = Side::PLAYER;
	}

	entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e) {
		return e->isDead && dynamic_cast<Unit*>(e.get());
		}), entities.end());

	if (!isClient) {
		// Optionnel : Désactiver l'IA si vous voulez tester le 1v1 pur
		// Pour l'instant on la laisse mais avec une condition
		/*
		if (enemyElixir >= 4.0f) {
			spawnUnit(Side::ENEMY, sf::Vector2f(200 + rand() % 400, 100), sf::Color::Magenta);
			enemyElixir -= 4.0f;
		}
		*/
	}
}

void Battle::draw(sf::RenderWindow& window) {
	// Le dessin utilise des coordonnées virtuelles (0-800, 0-1000)
	sf::RectangleShape map(sf::Vector2f(600, 800));
	map.setPosition(sf::Vector2f(100, 0));
	map.setFillColor(sf::Color(34, 139, 34));
	window.draw(map);

	sf::RectangleShape river(sf::Vector2f(600, 40));
	river.setPosition(sf::Vector2f(100, 380));
	river.setFillColor(sf::Color::Blue);
	window.draw(river);

	for (auto& e : entities) {
		e->draw(window);
	}

	// UI en bas (toujours du point de vue du joueur local)
	// On désactive temporairement la vue retournée pour l'UI
	sf::View currentView = window.getView();
	sf::View uiView(sf::FloatRect({0, 0}, {800, 1000}));
	window.setView(uiView);

	sf::RectangleShape uiBar(sf::Vector2f(800, 200));
	uiBar.setPosition(sf::Vector2f(0, 800));
	uiBar.setFillColor(sf::Color(50, 50, 50));
	window.draw(uiBar);

	float myElixir = (clientSide == Side::PLAYER) ? playerElixir : enemyElixir;
	sf::RectangleShape elixirBar(sf::Vector2f(myElixir * 60, 20));
	elixirBar.setPosition(sf::Vector2f(100, 820));
	elixirBar.setFillColor(sf::Color(255, 0, 255));
	window.draw(elixirBar);

	for (int i = 0; i < playerHand.size(); ++i) {
		sf::RectangleShape cardShape(sf::Vector2f(100, 140));
		cardShape.setPosition(sf::Vector2f(100 + i * 150, 850));
		cardShape.setFillColor(playerHand[i].color);
		if (selectedCardIndex == i) {
			cardShape.setOutlineThickness(5);
			cardShape.setOutlineColor(sf::Color::White);
		}
		window.draw(cardShape);
	}

	if (gameOver) {
		sf::RectangleShape overlay(sf::Vector2f(800, 1000));
		overlay.setFillColor(sf::Color(0, 0, 0, 150));
		window.draw(overlay);
	}

	window.setView(currentView); // Restaurer la vue
}

void Battle::spawnUnit(Side side, sf::Vector2f position, sf::Color color) {
	entities.push_back(std::make_unique<Unit>(position, 500, 100, 30, 50, side, color));
}

void Battle::handleServerSpawn(Side side, sf::Vector2f position, int cardIndex) {
	if (cardIndex < 0 || cardIndex >= playerHand.size()) return;

	float cost = (float)playerHand[cardIndex].cost;
	float& elixir = (side == Side::PLAYER) ? playerElixir : enemyElixir;

	if (elixir >= cost) {
		elixir -= cost;
		spawnUnit(side, position, playerHand[cardIndex].color);
	}
}

void Battle::updateElixir(float dt) {
	if (playerElixir < 10) playerElixir += 0.5f * dt;
	if (enemyElixir < 10) enemyElixir += 0.5f * dt;
}

std::string Battle::serialize() {
	game::GameState state;
	state.set_player_elixir(playerElixir);
	state.set_enemy_elixir(enemyElixir);
	state.set_game_over(gameOver);
	state.set_winner(static_cast<int32_t>(winner));

	for (auto& e : entities) {
		auto* entityState = state.add_entities();
		entityState->set_id(e->id);
		entityState->mutable_position()->set_x(e->position.x);
		entityState->mutable_position()->set_y(e->position.y);
		entityState->set_health(e->health);
		entityState->set_max_health(e->maxHealth);
		entityState->set_side(static_cast<int32_t>(e->side));
		entityState->set_is_tower(dynamic_cast<Tower*>(e.get()) != nullptr);
		
		sf::Color c = sf::Color::White;
		if (auto* u = dynamic_cast<Unit*>(e.get())) c = u->color;
		else if (auto* t = dynamic_cast<Tower*>(e.get())) c = t->shape.getFillColor();
		
		uint32_t color_rgb = (c.r << 16) | (c.g << 8) | c.b;
		entityState->set_color_rgb(color_rgb);
	}

	return state.SerializeAsString();
}

void Battle::deserialize(const std::string& data) {
	game::GameState state;
	if (!state.ParseFromString(data)) return;

	playerElixir = state.player_elixir();
	enemyElixir = state.enemy_elixir();
	gameOver = state.game_over();
	winner = static_cast<Side>(state.winner());

	entities.clear();
	for (const auto& entityState : state.entities()) {
		sf::Vector2f pos(entityState.position().x(), entityState.position().y());
		Side side = static_cast<Side>(entityState.side());
		
		if (entityState.is_tower()) {
			auto tower = std::make_unique<Tower>(pos, entityState.health(), 0, 0, side);
			tower->id = entityState.id();
			tower->maxHealth = entityState.max_health();
			entities.push_back(std::move(tower));
		} else {
			uint32_t rgb = entityState.color_rgb();
			sf::Color color((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
			auto unit = std::make_unique<Unit>(pos, entityState.health(), 0, 0, 0, side, color);
			unit->id = entityState.id();
			unit->maxHealth = entityState.max_health();
			entities.push_back(std::move(unit));
		}
	}
}

void Battle::setNetwork(sf::UdpSocket* s, sf::IpAddress ip, std::uint16_t port) {
	socketPointer = s;
	serverIp = ip;
	serverPort = port;
}

void Battle::handleInput(sf::RenderWindow& window, const sf::Event& event, const sf::View& gameView) {
	if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
		if (mouseButton->button == sf::Mouse::Button::Left) {
			// Coordonnées pour l'UI (View standard 800x1000)
			sf::View uiView(sf::FloatRect({0, 0}, {800, 1000}));
			window.setView(uiView);
			sf::Vector2f uiPos = window.mapPixelToCoords(mouseButton->position);
			
			for (int i = 0; i < playerHand.size(); ++i) {
				sf::FloatRect cardBounds({ float(100 + i * 150), 850.f }, { 100.f, 140.f });
				if (cardBounds.contains(uiPos)) {
					selectedCardIndex = i;
					window.setView(gameView);
					return;
				}
			}

			// Coordonnées pour le terrain (Game View qui peut être retournée)
			window.setView(gameView);
			sf::Vector2f mousePos = window.mapPixelToCoords(mouseButton->position);

			if (selectedCardIndex != -1 && mousePos.y < 800) {
				if (isClient && socketPointer) {
					game::SpawnRequest request;
					request.set_card_index(selectedCardIndex);
					request.mutable_position()->set_x(mousePos.x);
					request.mutable_position()->set_y(mousePos.y);
					
					std::string data = request.SerializeAsString();
					sf::Packet packet;
					packet << data;
					socketPointer->send(packet, serverIp, serverPort);
				}
			}
		}
	}
}
