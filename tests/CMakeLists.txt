include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/.subtree/Catch2/single_include)

set( TEST_SRC
  "test_poker.cpp"
  "test_title.cpp"
  )

add_library(libtest_rankup SHARED ${TEST_SRC})
set_target_properties( libtest_rankup
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)

add_executable(test_rankup ${CMAKE_SOURCE_DIR}/tests/main.cpp)
target_link_libraries(test_rankup libtest_rankup)
set_target_properties( test_rankup
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
