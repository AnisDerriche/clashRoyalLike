# Étape 1 : Build
FROM ubuntu:22.04 AS builder

# Éviter les prompts interactifs pendant l'installation
ENV DEBIAN_FRONTEND=noninteractive

# Installation des dépendances système pour compiler et pour vcpkg / SFML
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    ninja-build \
    libx11-dev \
    libxrandr-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libflac-dev \
    libogg-dev \
    libvorbis-dev \
    libopenal-dev \
    libfreetype6-dev \
    libpq-dev \
    libssl-dev

WORKDIR /app

# Copie des fichiers sources
COPY . /app

# Configuration de vcpkg
ENV VCPKG_ROOT=/app/vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT && \
    $VCPKG_ROOT/bootstrap-vcpkg.sh

# Configuration et build de la cible BattleServer (on skip le client)
RUN cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DVCPKG_TARGET_TRIPLET=x64-linux

RUN cmake --build build --target BattleServer -j$(nproc)

# Étape 2 : Runtime (Production)
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Installation des dépendances runtime uniquement
RUN apt-get update && apt-get install -y \
    libx11-6 \
    libxrandr2 \
    libudev1 \
    libgl1 \
    libflac8 \
    libogg0 \
    libvorbis0a \
    libvorbisenc2 \
    libvorbisfile3 \
    libopenal1 \
    libfreetype6 \
    libpq5 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copie de l'exécutable depuis l'étape de build
COPY --from=builder /app/build/backend/battle-server/BattleServer /app/BattleServer

# Exposition du port UDP du serveur
EXPOSE 50000/udp

# Lancement du serveur
CMD ["./BattleServer"]