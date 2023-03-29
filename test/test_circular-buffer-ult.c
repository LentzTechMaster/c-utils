#include "unity.h"
#include "circular-buffer-ultimate.h"

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


circ_buf_t buffer;
#define buffer_size 10
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
    circ_buf_init_buffer(&buffer, buffer_size, 512);
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
}

void test_full()
{
    for (int32_t i = 0; i < buffer_size; i++)
    {
        if(i%2==0)
        {
           TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_A_unsafe());
        }
        else
        {
            TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_B_unsafe());
        }
    }

    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_FULL, add_buff_A_unsafe());
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_is_full(&buffer));
    circ_buf_elem_t elem;

    circ_buf_pop_unsafe(&buffer, &elem);

    A_struct_t rcvd;
    memcpy(&rcvd, elem.pointer, elem.size);
    free(elem.pointer);

    TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, add_buff_B_unsafe());
}

void test_memory_full()
{
    circ_buf_t b;
    #define MAX_MEM_SIZE 8
    circ_buf_init_buffer(&b, 100, MAX_MEM_SIZE);

    A_struct_t a;
    a.a1 = 50;
    a.a2 = 1234567890;

    for (uint32_t i = 0; i < MAX_MEM_SIZE/sizeof(A_struct_t); i++)
    {
        TEST_ASSERT_EQUAL_UINT8(CB_SUCCESS, circ_buf_push_safe(&b, &a, sizeof(A_struct_t), 0));
    }
    TEST_ASSERT_EQUAL_UINT8(CB_BUFFER_MAX_MEMORY_REACHED, circ_buf_push_safe(&b, &a, sizeof(A_struct_t), 0));

}


