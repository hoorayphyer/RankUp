cmake_minimum_required(VERSION 3.12)
project (RankUp)

# Enable testing
enable_testing(true)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-std=c++14 -pthread -O3 -g -DNDEBUG -fPIC -march=native")
#set(CMAKE_CXX_FLAGS "-Wl,--out-implib")
add_subdirectory(rules)
