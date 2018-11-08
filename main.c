#include <stdio.h>
#include <assert.h>
#include "utils/circular-byte-buffer.h"

#define BUFFER_SIZE 16
CIRC_BBUF_DEF(my_circ_buf, BUFFER_SIZE);

void test_buffer();
void print_buffer();

int main() {

    test_buffer();

    return 0;
}

void test_buffer()
{
    uint8_t data[11] = {10,20,30,40,50,60,70,80,90,100,110};
    uint8_t data2[13] = {255,11,254,12,253,13,252,14,251,15,250,16,249};
    uint8_t out_data = 0;
    uint8_t out_data2[13];
    uint8_t index = 0;

    // Insert byte by byte into buffer
    for (uint8_t i = 0; i < sizeof(data); ++i) {
        if (circ_bbuf_push(&my_circ_buf, data[i])) {
            printf("Out of space in CB\n");
            break;
        }
    }
    assert(circ_bbuf_available_space(&my_circ_buf) == BUFFER_SIZE - sizeof(data));

    // Retrieve byte by byte from the buffer
    while (circ_bbuf_pop(&my_circ_buf, &out_data) == CBB_SUCCESS)
    {
        printf("[%u]Out_data : %u\n", index, out_data);
        assert(data[index] == out_data);
        out_data = 0;
        index++;
    }
    assert(index == sizeof(data));

    // Push multiples bytes into buffer
    circ_bbuf_push_bytes(&my_circ_buf, data2, sizeof(data2));
    print_buffer();

    // Retrieve multiple bytes from buffer
    circ_bbuf_pop_bytes(&my_circ_buf, sizeof(data2), out_data2);
    for (uint32_t i = 0;  i < sizeof(data2); i++) {
        printf("[%u]Out_data2 : %u\n", i, out_data2[i]);
        assert(out_data2[i] == data2[i]);
    }
}

void print_buffer()
{
    printf("----------------\n");
    printf("Head: %u Tail: %u\n", my_circ_buf.head, my_circ_buf.tail);
    printf("Content of buffer : \n");
    for(uint32_t i=0; i<BUFFER_SIZE; i++)
    {
        printf("[%u] %u\n", i, my_circ_buf.buffer[i]);
    }
    printf("----------------\n");
}