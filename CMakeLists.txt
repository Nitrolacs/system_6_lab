cmake_minimum_required(VERSION 3.24)
project(5_lab C)

set(CMAKE_C_STANDARD 99)

add_executable(5_lab server.c server.h client.c client.h interface.c interface.h logic.c logic.h)
