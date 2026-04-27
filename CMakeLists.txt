cmake_minimum_required(VERSION 3.20)
project(clashRoyalLike) 

# Ajout du dossier cmake local pour les correctifs (ex: Brotli)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Forcer le runtime MSVC statique (MT pour Release, MTd pour Debug)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Indiquer à SFML qu'on utilise la version statique
add_definitions(-DSFML_STATIC)

set(CMAKE_CXX_STANDARD 20)

# --- SFML 3 ---
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window Network Audio System)

# --- REDIS ---
find_package(hiredis CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)

# --- POSTGRES ---
find_package(libpqxx CONFIG REQUIRED)

# --- BACKEND & TOOLS ---
find_package(Drogon CONFIG REQUIRED)
find_package(Protobuf REQUIRED)

# --- INCLUSION DES DOSSIERS ---
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(backend/api-service)
add_subdirectory(backend/battle-server)
