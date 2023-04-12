#include "unity.h"
#include "circular-buffer-flex.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define b2_size 5
typedef struct {
    char b1;
    uint32_t b2[b2_size];
} B_struct_t;


circ_buf_flex_t buffer;
#define buffer_size 32
static const uint8_t FALSE = 0;
static const uint8_t TRUE = 1;

void setUp(void)
{
    circ_buf_flex_init_buffer(&buffer, buffer_size, sizeof(B_struct_t));
}

void tearDown(void)
{
    circ_buf_flex_reset(&buffer);
}
void test_empty()
{
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_empty(&buffer));

}


void test_push_pop()
{
    B_struct_t b1;
    b1.b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b1.b2[i] = i * i;
    }
    

    B_struct_t b2;
    b2.b1 = 'T';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b2.b2[i] = i * i/2;
    }

    
    circ_buf_flex_push(&buffer, &b1);
    circ_buf_flex_push(&buffer, &b2);

   
    B_struct_t b;
    circ_buf_flex_pop(&buffer, &b);
    TEST_ASSERT_EQUAL_CHAR('D', b.b1);

    for (int i = 0; i < b2_size; i++)
    {
        
        TEST_ASSERT_EQUAL_UINT32(i*i, b.b2[i]);
    }

    circ_buf_flex_pop(&buffer, &b);
    TEST_ASSERT_EQUAL_CHAR('T', b.b1);

    for (int i = 0; i < b2_size; i++)
    {
        
        TEST_ASSERT_EQUAL_UINT32(i*i/2, b.b2[i]);
    }

    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_empty(&buffer));
}

void test_pop_using_available_elements_to_read()
{
    B_struct_t b1;
    b1.b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b1.b2[i] = i * i;
    }
    

    B_struct_t b2;
    b2.b1 = 'T';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b2.b2[i] = i * i/2;
    }
    for (uint8_t i = 0; i < buffer_size/2; i++)
    {
        circ_buf_flex_push(&buffer, &b1);
        circ_buf_flex_push(&buffer, &b2);
    }
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_full(&buffer));

    while(circ_buf_flex_available_elements_to_read(&buffer))
    {
        B_struct_t b;
        circ_buf_flex_pop(&buffer, &b);
    }
    
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_empty(&buffer));

}

void test_full()
{
    B_struct_t b1;
    b1.b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b1.b2[i] = i * i;
    }
    

    B_struct_t b2;
    b2.b1 = 'T';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b2.b2[i] = i * i/2;
    }
    for (uint8_t i = 0; i < buffer_size/2; i++)
    {
        circ_buf_flex_push(&buffer, &b1);
        circ_buf_flex_push(&buffer, &b2);
    }
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_full(&buffer));
    
    TEST_ASSERT_EQUAL_UINT8(CBF_BUFFER_FULL, circ_buf_flex_push(&buffer, &b1));
}

void test_empty_pop()
{
    test_pop_using_available_elements_to_read();
    B_struct_t b;
    TEST_ASSERT_EQUAL_UINT8(CBF_BUFFER_EMPTY, circ_buf_flex_pop(&buffer, &b));

}

void test_drop()
{
    B_struct_t b1;
    b1.b1 = 'D';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b1.b2[i] = i * i;
    }
    

    B_struct_t b2;
    b2.b1 = 'T';
    for (uint8_t i = 0; i < b2_size; i++)
    {
        b2.b2[i] = i * i/2;
    }
    
    TEST_ASSERT_EQUAL_UINT32(0, circ_buf_flex_available_elements_to_read(&buffer));
    
    circ_buf_flex_push(&buffer, &b1);
    circ_buf_flex_push(&buffer, &b2);

    TEST_ASSERT_EQUAL_UINT32(2, circ_buf_flex_available_elements_to_read(&buffer));
   
    circ_buf_flex_drop(&buffer);

    
    TEST_ASSERT_EQUAL_UINT32(1, circ_buf_flex_available_elements_to_read(&buffer));

    
    B_struct_t b;
    circ_buf_flex_pop(&buffer, &b);
    TEST_ASSERT_EQUAL_CHAR('T', b.b1);

    for (int i = 0; i < b2_size; i++)
    {
        
        TEST_ASSERT_EQUAL_UINT32(i*i/2, b.b2[i]);
    }

    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_buf_flex_is_empty(&buffer));
}