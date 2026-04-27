#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <memory>
#include <string>

enum class Side {
    PLAYER,
    ENEMY
};

struct Entity {
    uint32_t id;
    sf::Vector2f position;
    float health;
    float maxHealth;
    float damage;
    float range;
    Side side;
    bool isDead = false;

    static uint32_t nextId;
    Entity() : id(nextId++) {}
    virtual ~Entity() = default;
    virtual void update(float dt, std::vector<std::unique_ptr<Entity>>& entities) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

struct Unit : public Entity {
    float speed;
    sf::Color color;
    Entity* target = nullptr;

    Unit(sf::Vector2f pos, float hp, float dmg, float rng, float spd, Side s, sf::Color c);
    void update(float dt, std::vector<std::unique_ptr<Entity>>& entities) override;
    void draw(sf::RenderWindow& window) override;
    void findTarget(std::vector<std::unique_ptr<Entity>>& entities);
};

struct Tower : public Entity {
    sf::RectangleShape shape;

    Tower(sf::Vector2f pos, float hp, float dmg, float rng, Side s);
    void update(float dt, std::vector<std::unique_ptr<Entity>>& entities) override;
    void draw(sf::RenderWindow& window) override;
};

struct Card {
    std::string name;
    int cost;
    sf::Color color;
};

class Battle {
public:
    Battle(bool isClient = false);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void spawnUnit(Side side, sf::Vector2f position, sf::Color color);
    void handleServerSpawn(Side side, sf::Vector2f position, int cardIndex);
    void handleInput(sf::RenderWindow& window, const sf::Event& event, const sf::View& gameView);

    // Networking
    void setClientSide(Side side) { clientSide = side; }
    Side getClientSide() const { return clientSide; }
    std::string serialize();
    void deserialize(const std::string& data);
    void setNetwork(sf::UdpSocket* socket, sf::IpAddress ip, std::uint16_t port);

private:
    std::vector<std::unique_ptr<Entity>> entities;
    float playerElixir;
    float enemyElixir;
    std::vector<Card> playerHand;
    int selectedCardIndex = -1;
    bool gameOver = false;
    Side winner;

    bool isClient;
    Side clientSide = Side::PLAYER; // Par défaut joueur 1
    sf::UdpSocket* socketPointer = nullptr;
    sf::IpAddress serverIp;
    std::uint16_t serverPort;

    void updateElixir(float dt);
};
