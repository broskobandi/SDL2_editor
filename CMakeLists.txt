cmake_minimum_required(VERSION 3.20)
project(SDL2_editor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(test EXCLUDE_FROM_ALL test/test.cpp)
target_link_libraries(test PRIVATE ctest SDL2)
target_compile_options(test PRIVATE -Wall -Wextra -Werror -Wunused-result -Wconversion)
target_compile_definitions(test PRIVATE TEST)
target_include_directories(test PRIVATE src)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wunused-result -Wconversion)
