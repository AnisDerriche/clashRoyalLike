
add_executable(RoyalArenaClient 
    src/main.cpp
    src/states/states.cpp
    src/engine/menu.cpp
    include/menu.h
    include/states.h
)

target_include_directories(RoyalArenaClient PRIVATE 
    include 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common
)

target_link_libraries(RoyalArenaClient PRIVATE 
    Common 
    SFML::Graphics 
    SFML::Window 
    SFML::Network 
    SFML::Audio 
    SFML::System
)