cmake_minimum_required(VERSION 3.20)
project(clashRoyalLike)

set(CMAKE_CXX_STANDARD 17)

# On inclut le dossier où sont tes fichiers .h
include_directories(clashRoyalLike/include)

# Active le support Unicode pour éviter les erreurs de L"String"
add_definitions(-DUNICODE -D_UNICODE)

# On liste tous tes fichiers sources .cpp
# Note l'ajout de WIN32 pour supprimer la console
add_executable(clashRoyalLike WIN32
        clashRoyalLike/src/game.cpp
        clashRoyalLike/src/platform_common.cpp
        clashRoyalLike/src/renderer.cpp
        clashRoyalLike/src/utils.cpp
        clashRoyalLike/src/w32_platform.cpp
)

# Le "Hack" pour garder le int main() classique sans erreur de Linker
if(MSVC)
    set_target_properties(clashRoyalLike PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

# Indispensable pour WinMain et l'affichage graphique sous Windows
target_link_libraries(clashRoyalLike user32 gdi32)