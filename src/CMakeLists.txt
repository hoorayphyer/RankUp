include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(librankup SHARED "poker.cpp" "pattern.cpp")
set_target_properties( librankup
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
)

add_executable(rankup "main.cpp")
target_link_libraries(rankup librankup)
set_target_properties( rankup
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
