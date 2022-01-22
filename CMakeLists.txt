cmake_minimum_required(VERSION 3.12)
project (RankUp)

# Enable testing
enable_testing(true)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-std=c++17 -pthread -O3 -g -DNDEBUG -fPIC -march=native")
#set(CMAKE_CXX_FLAGS "-Wl,--out-implib")

function(test_gen category name)
  set(test_target "test_${category}_${name}")
  add_executable( ${test_target} "tests/test_${name}.cpp")
  target_compile_options( ${test_target} PRIVATE -O0 ) # -Wmissing-braces
  # ${CMAKE_CURRENT_LIST_DIR} becomes the one in which test_gen is called
  target_include_directories( ${test_target} PRIVATE ${CMAKE_CURRENT_LIST_DIR} )

  foreach( lib IN LISTS ARGN)
    target_link_libraries(${test_target} PRIVATE ${lib})
  endforeach()
  target_link_libraries( ${test_target} PUBLIC catchtf )
  add_test( NAME TEST_${category}_${name} COMMAND ${test_target} )
endfunction()

add_subdirectory(catchtf)
add_subdirectory(rules)
