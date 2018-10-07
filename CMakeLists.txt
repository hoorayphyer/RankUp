cmake_minimum_required(VERSION 2.8)
project (RankUp)

# Enable testing
enable_testing(true)

set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -O3 -g -DNDEBUG -Wall -Wextra -fPIC -march=native ${CXX_EXTRA_FLAGS}")

add_subdirectory(src)
add_subdirectory(tests)
