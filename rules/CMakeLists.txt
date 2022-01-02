add_library(rankup_rules SHARED rules.cpp rules_impl.cpp)
target_include_directories(rankup_rules PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)