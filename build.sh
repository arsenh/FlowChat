#!/bin/zsh

set -e  # stop on first error

BIN_DIR="./bin"

# ensure bin directory exists
mkdir -p "$BIN_DIR"

build_client() {
    echo "Building Go client..."
    cd client
    go build -o "../bin/flow_chat_client" ./cmd/main.go
    cd ..
    echo "Client built: bin/flow_chat_client"
}

build_server() {
    echo "Building C++ server..."

    cd server

    # Create build directory if missing
    mkdir -p build

    # Conan install
    conan install . \
        --output-folder=build \
        --build=missing

    cd build

    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Debug

    cmake --build .

    # Copy server executable to top bin/
    cp flow_chat_server ../../bin/

    cd ../..
    echo "Server built: bin/flow_chat_server"
}

clean_all() {
    echo "Cleaning build artifacts..."

    # Remove bin directory
    if [ -d "$BIN_DIR" ]; then
        rm -rf "$BIN_DIR"
        echo "Removed: bin/"
    fi

    # Remove server build directory
    if [ -d "server/build" ]; then
        rm -rf "server/build"
        echo "Removed: server/build/"
    fi

    echo "Clean complete."
}

case "$1" in
    client)
        build_client
        ;;
    server)
        build_server
        ;;
    all)
        build_client
        build_server
        ;;
    clean)
        clean_all
        ;;
    *)
        echo "Usage: ./build.sh {client|server|all|clean}"
        exit 1
        ;;
esac

