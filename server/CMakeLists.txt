cmake_minimum_required(VERSION 3.16)
project(flow_chat_server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED)

# Include directories
include_directories(
        ${CMAKE_SOURCE_DIR}/src
)

# Source files
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/tcp/tcp_server.cpp
)

# Link Crow after target creation
target_link_libraries(${PROJECT_NAME} PRIVATE boost::boost pthread)

