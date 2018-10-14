cmake_minimum_required(VERSION 2.8)
project (RankUp)

# Enable testing
enable_testing(true)

set(CMAKE_BUILD_TYPE Release)

#set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -O3 -g -DNDEBUG -fPIC -march=native")
#set(CMAKE_CXX_FLAGS "-Wl,--out-implib")
add_subdirectory(src)
add_subdirectory(tests)
