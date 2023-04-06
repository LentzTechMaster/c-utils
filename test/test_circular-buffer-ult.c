#include "unity.h"
#include "circular-buffer-ultimate.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef CEEDLING_WARNING_MEMCPY
#include <string.h>
#endif

typedef struct {
    uint8_t a1;
    uint32_t a2;
} A_struct_t;

#define b2_size 5
typedef struct {
    char b1;
    uint32_t b2[b2_size];
} B_struct_t;

typedef struct {
    uint64_t l0;
    uint64_t l1;
    uint64_t l2;
    uint64_t l3;
    uint64_t l4;
    uint64_t l5;
    uint64_t l6;
    uint64_t l7;
} LARGE_struct_t;


circ_buf_t buffer;
#define buffer_size 10
#define buffer_memory 512
static const uint8_t FALSE = 0;
static const uint8_t TRUE = 1;

uint8_t add_buff_A_unsafe()
{
    A_struct_t* a = (A_struct_t*)malloc(sizeof(A_struct_t));
    a->a1 = 50;
    a->a2 = 1234567890;

    return circ_buf_push_unsafe(&buffer, a, sizeof(A_struct_t), 0);
}

uint8_t add_buff_B_unsafe()
{
    B_struct_t* b = (B_struct_t*)malloc(sizeof(B_struct_t));
    b->b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b->b2[i] = i * i;
    }

    return circ_buf_push_unsafe(&buffer, b, sizeof(B_struct_t), 1);
}


uint8_t add_buff_A_safe()
{
    A_struct_t a;
    a.a1 = 50;
    a.a2 = 1234567890;

    return circ_buf_push_safe(&buffer, &a, sizeof(A_struct_t), 0);
}

uint8_t add_buff_B_safe()
{
    B_struct_t b;
    b.b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b.b2[i] = i * i;
    }

    return circ_buf_push_safe(&buffer, &b, sizeof(B_struct_t), 1);
}
void setUp(void)
{
    circ_buf_init_buffer(&buffer, buffer_size, buffer_memory);
}

void tearDown(void)
{
    circ_buf_reset_buffer(&buffer);
}
void test_empty()
{
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));

}

void test_memory_size()
{
    add_buff_A_unsafe();
    TEST_ASSERT_EQUAL_UINT8(sizeof(A_struct_t), buffer.memory_size);
}

void test_pop_unsafe()
{

    add_buff_A_unsafe();
    add_buff_B_unsafe();

    circ_buf_elem_t elem;

    circ_buf_pop_unsafe(&buffer, &elem);

    A_struct_t rcvd;
    memcpy(&rcvd, elem.pointer, elem.size);
    free(elem.pointer);
    TEST_ASSERT_EQUAL_UINT32(sizeof(A_struct_t), elem.size);
    TEST_ASSERT_EQUAL_UINT32(0, elem.type);

    TEST_ASSERT_EQUAL_UINT8(50, rcvd.a1);
    TEST_ASSERT_EQUAL_UINT32(1234567890, rcvd.a2);

    circ_buf_pop_unsafe(&buffer, &elem);
    B_struct_t rcvd_B;
    memcpy(&rcvd_B, elem.pointer, elem.size);
    free(elem.pointer);
    TEST_ASSERT_EQUAL_UINT32(sizeof(B_struct_t), elem.size);
    TEST_ASSERT_EQUAL_UINT32(1, elem.type);

    TEST_ASSERT_EQUAL_CHAR('D', rcvd_B.b1);

    for (int i = 0; i < b2_size; i++)
    {
        
        TEST_ASSERT_EQUAL_UINT32(i*i, rcvd_B.b2[i]);
    }
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));
}

void test_pop_safe()
{
    add_buff_A_safe();
    add_buff_B_safe();

   

    A_struct_t rcvd_a;

    circ_buf_pop_safe(&buffer, &rcvd_a);

    TEST_ASSERT_EQUAL_UINT8(50, rcvd_a.a1);
    TEST_ASSERT_EQUAL_UINT32(1234567890, rcvd_a.a2);

    B_struct_t rcvd_B;
    circ_buf_pop_safe(&buffer, &rcvd_B);
    

    TEST_ASSERT_EQUAL_CHAR('D', rcvd_B.b1);

    for (int i = 0; i < b2_size; i++)
    {
        
        TEST_ASSERT_EQUAL_UINT32(i*i, rcvd_B.b2[i]);
    }
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, buffer.memory_size);
}

void test_full()
{
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));
    for (int32_t i = 0; i < buffer_size; i++)
    {
        if(i%2==0)
        {
           TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_A_safe());
        }
        else
        {
            TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_B_safe());
        }
    }

    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_FULL, add_buff_A_safe());
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_full(&buffer));
    
    A_struct_t elem;

    circ_buf_pop_safe(&buffer, &elem);
    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_FILLING, buffer.buffer_status);

    TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_B_unsafe());
    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_FULL, buffer.buffer_status);
}

void test_reset()
{
    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_EMPTY, buffer.buffer_status);
    test_full();
    circ_buf_reset_buffer(&buffer);
    TEST_ASSERT_EQUAL_UINT8(buffer_size, buffer.capacity);
    TEST_ASSERT_EQUAL_UINT32(buffer_memory, buffer.max_memory_size);
    TEST_ASSERT_EQUAL_UINT32(0, buffer.memory_size);
    TEST_ASSERT_EQUAL_UINT32(0, buffer.head);
    TEST_ASSERT_EQUAL_UINT32(0, buffer.tail);
}

void test_memory_full()
{
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));
    LARGE_struct_t large;
    large.l1 = 0;
    large.l2 = 1;
    large.l3 = 2;
    large.l4 = 3;
    large.l5 = 4;
    large.l6 = 5;
    large.l7 = 6;

    for (uint32_t i = 0; i < buffer_memory/sizeof(LARGE_struct_t)/*THIS SHOULD NOT BE HIGHER THAN buffer_size in order to test*/; i++)
    {
        TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, circ_buf_push_safe(&buffer, &large, sizeof(LARGE_struct_t), 0));
    }
    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_MAX_MEMORY_REACHED, circ_buf_push_safe(&buffer, &large, sizeof(LARGE_struct_t), 0));
}

void test_peek()
{
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_empty(&buffer));
    
    for (int32_t i = 0; i < buffer_size; i++)
    {
        srand(time(NULL));   // Initialization, should only be called once.
        uint8_t r = rand()%2;      // Returns a pseudo-random integer between 0 and RAND_MAX.
        if(r==0)
        {
           TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_A_safe());
        }
        else
        {
            TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_B_safe());
        }
    }

    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_FULL, add_buff_A_safe());
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_full(&buffer));

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_buf_available_elements_to_read(&buffer));
    uint32_t elem_num = circ_buf_available_elements_to_read(&buffer);
    for (uint32_t i = 0; i < elem_num; i++)
    {
        uint32_t size = 0;
        uint32_t type = 3;
        TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, circ_buf_peek_size_and_type(&buffer, &size, &type));
        if(type==0)
        {
            TEST_ASSERT_EQUAL_UINT32(sizeof(A_struct_t), size);
            A_struct_t a;
            TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, circ_buf_pop_safe(&buffer, &a));
        }
        else if(type==1)
        {
            TEST_ASSERT_EQUAL_UINT32(sizeof(B_struct_t), size);
            B_struct_t b;
            TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, circ_buf_pop_safe(&buffer, &b));
        }
        TEST_ASSERT_EQUAL_UINT32(buffer_size-i-1, circ_buf_available_elements_to_read(&buffer));
    }
    TEST_ASSERT_EQUAL_UINT8(0, circ_buf_available_elements_to_read(&buffer));
}


