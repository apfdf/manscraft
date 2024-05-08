cmake_minimum_required(VERSION 3.5.0)
project(manscraft VERSION 0.1.0)

add_executable(manscraft src/main.cpp)

target_link_libraries(manscraft glfw GLEW GL)