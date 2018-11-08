cmake_minimum_required(VERSION 3.12)
project(circular_byte_buffer C)

set(CMAKE_C_STANDARD 99)

add_executable(circular_byte_buffer main.c utils/circular-byte-buffer.h utils/circular-byte-buffer.c)