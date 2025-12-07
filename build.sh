#!/bin/zsh
set -e  # stop on first error

BIN_DIR="./bin"

# ensure bin directory exists
mkdir -p "$BIN_DIR"

build_client() {
    echo "Building Java client..."
    cd client

    # Build with Maven wrapper
    ./mvnw clean package -DskipTests

    # Find the generated JAR (assuming it's in target/)
    JAR_FILE=$(find target -name "*.jar" -not -name "*-sources.jar" -not -name "*-javadoc.jar" | head -n 1)

    if [ -z "$JAR_FILE" ]; then
        echo "Error: No JAR file found in target/"
        exit 1
    fi

    # Copy JAR to bin directory
    cp "$JAR_FILE" "../bin/flow_chat_client.jar"

    # Create executable script
    cat > "../bin/flow_chat_client" << 'EOF'
#!/bin/zsh
DIR="$(cd "$(dirname "$0")" && pwd)"
java -jar "$DIR/flow_chat_client.jar" "$@"
EOF

    chmod +x "../bin/flow_chat_client"

    cd ..
    echo "Client built: bin/flow_chat_client.jar"
    echo "Launcher script: bin/flow_chat_client"
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

    # Remove Maven target directory
    if [ -d "client/target" ]; then
        rm -rf "client/target"
        echo "Removed: client/target/"
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
