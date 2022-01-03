add_library(catchtf SHARED "catchtf_main.cpp")
target_include_directories(catchtf PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../.subtree/Catch2/single_include)