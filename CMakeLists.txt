cmake_minimum_required(VERSION 3.20)
# Change le nom ici pour la cohérence
project(clashRoyalLike) 

set(CMAKE_CXX_STANDARD 20)

# --- SFML 3 ---
find_package(SFML CONFIG REQUIRED COMPONENTS Graphics Window Network Audio System)

# --- REDIS (Ordre important pour éviter l'erreur hiredis) ---
find_package(hiredis CONFIG REQUIRED)
find_package(redis++ CONFIG REQUIRED)

# --- POSTGRES ---
find_package(libpqxx CONFIG REQUIRED)

# --- BACKEND & TOOLS ---
find_package(Drogon CONFIG REQUIRED)
find_package(Protobuf REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# --- INCLUSION DES DOSSIERS ---
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(backend/api-service)