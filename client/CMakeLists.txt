add_executable(RoyalArenaClient src/main.cpp)

target_link_libraries(RoyalArenaClient PRIVATE 
    Common 
    SFML::Graphics 
    SFML::Window 
    SFML::Network 
    SFML::Audio 
    SFML::System
)