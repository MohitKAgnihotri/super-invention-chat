cmake_minimum_required(VERSION 3.16.3)
project(super-invention-chat C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
link_libraries(pthread)

add_executable(chat_server
        chat_server.c
        chat_server.h chat_message.h user_interface.c user_interface.h)

add_executable(daytime_RFC_867_Client
        chat_client.c
        chat_client.h)