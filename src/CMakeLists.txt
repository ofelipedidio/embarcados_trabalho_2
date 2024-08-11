add_library(vec2 STATIC vec2.c)
target_include_directories(vec2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(energy energia.c)
target_include_directories(energy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main main.c)
target_link_libraries(main vec2 energy)
