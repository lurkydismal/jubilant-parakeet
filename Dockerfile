FROM alpine

# Ordered by size
RUN apk add \
    make pkgconf bash fd mold git \
    gtest gmock \
    xxhash mimalloc libelf libunwind zstd-libs \
    llvm20 clang20 clang20-extra-tools \
    build-base cmake samurai ccache

# Build snappy
WORKDIR /tmp

RUN git clone --depth=1 --single-branch https://github.com/google/snappy.git snappy

WORKDIR /tmp/snappy

RUN git submodule update --init --depth 1

RUN export CC="clang" && \
    export CXX="clang++" && \
    cmake -B build -G Ninja \
    -Wno-dev \
    -DCMAKE_LINKER=mold \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build && \
    cmake --install build

# Build SDL3, SDL3 image and SDL3 ttf
WORKDIR /tmp

RUN git clone --depth=1 --single-branch https://github.com/libsdl-org/SDL.git SDL3

RUN apk add \
    libx11-dev libxext-dev libxrandr-dev libxrender-dev libxcursor-dev libxfixes-dev libxi-dev libxinerama-dev

RUN git clone --depth=1 --single-branch https://github.com/libsdl-org/SDL_image.git SDL3_image

RUN git clone --depth=1 --single-branch https://github.com/libsdl-org/SDL_ttf.git SDL3_ttf

# Build SDL3
WORKDIR /tmp/SDL3

RUN export CC="clang" && \
    export CXX="clang++" && \
    cmake -B build -G Ninja \
    -DCMAKE_LINKER=mold \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build && \
    cmake --install build

# Build SDL3 image
WORKDIR /tmp/SDL3_image

RUN export CC="clang" && \
    export CXX="clang++" && \
    cmake -B build -G Ninja \
    -DCMAKE_LINKER=mold \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build && \
    cmake --install build

# Build SDL3 ttf
WORKDIR /tmp/SDL3_ttf

RUN apk add \
    freetype-dev

RUN export CC="clang" && \
    export CXX="clang++" && \
    cmake -B build -G Ninja \
    -DCMAKE_LINKER=mold \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build && \
    cmake --install build

WORKDIR /build

COPY . .
