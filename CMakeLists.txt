cmake_minimum_required(VERSION 3.12)
project(circular_byte_buffer C)

set(CMAKE_C_STANDARD 99)

add_executable(c-utils main.c utils/circular-byte-buffer.h utils/circular-byte-buffer.c utils/timestamp-buffer.h utils/timestamp-buffer.c)