add_executable(RoyalArenaClient 
    src/main.cpp
    src/states/states.cpp
    src/engine/menu.cpp
    src/engine/game_logic.cpp
    include/menu.h
    include/states.h
    include/game_logic.h
)

target_include_directories(RoyalArenaClient PRIVATE 
    include 
)

target_link_libraries(RoyalArenaClient PRIVATE 
    Common 
    SFML::Graphics 
    SFML::Window 
    SFML::Network 
    SFML::Audio 
    SFML::System
)
