cmake_minimum_required(VERSION 3.24)
project(6_lab C)

set(CMAKE_C_STANDARD 99)

add_executable(6_lab server.c server.h client.c client.h interface.c interface.h logic.c logic.h signals.c signals.h)
