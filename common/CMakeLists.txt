# Trouver Protobuf via la config vcpkg
find_package(protobuf CONFIG REQUIRED)

# On définit les fichiers proto
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")

# 1. Générer les fichiers C++
# Cette macro remplit PROTO_SRCS et PROTO_HDRS avec les chemins des fichiers générés
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# 2. Créer la bibliothèque avec les fichiers générés
add_library(Common STATIC 
    src/dummy.cpp
    include/utils.h
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

# 3. Configurer les inclusions et les liens
target_include_directories(Common PUBLIC 
    include 
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(Common PUBLIC protobuf::libprotobuf)
