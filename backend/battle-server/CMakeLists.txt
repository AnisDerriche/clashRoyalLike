add_executable(BattleServer 
    src/main.cpp
    ../../client/src/engine/game_logic.cpp
)

target_include_directories(BattleServer PRIVATE 
    ../../client/include 
)

target_link_libraries(BattleServer PRIVATE 
    Common 
    SFML::Graphics
    SFML::Window
    SFML::Network 
    SFML::System
)
