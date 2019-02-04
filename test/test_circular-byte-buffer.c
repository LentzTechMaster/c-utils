#include "unity.h"
#include "circular-byte-buffer.h"

static circ_bbuf_t buffer;
static const uint32_t buffer_size = 8;
static const uint8_t FALSE = 0;
static const uint8_t TRUE = 1;

void setUp(void)
{
    circ_bbuf_create_buffer(&buffer, buffer_size);
}

void tearDown(void)
{
}

void test_cbb_reset_buffer()
{
    uint8_t input_array[]= {11,22,33,44,55,66};
    circ_bbuf_push_bytes(&buffer, input_array, sizeof(input_array));

    circ_bbuf_reset_buffer(&buffer);
    
    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_empty_buffer()
{
    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_insert_byte()
{
    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_push(&buffer, 0x10));
    TEST_ASSERT_EQUAL_UINT32(buffer_size -1, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(1, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_peek_read_byte()
{
    uint8_t poped_from_buffer = 0;
    circ_bbuf_push(&buffer, 0x10);

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_peek(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(0x10, poped_from_buffer);
    poped_from_buffer = 0;

    TEST_ASSERT_EQUAL_UINT32(buffer_size - 1, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(1, circ_bbuf_available_bytes_to_read(&buffer));

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(0x10, poped_from_buffer);

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_read_byte()
{
    uint8_t poped_from_buffer = 0;
    circ_bbuf_push(&buffer, 0x10);

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(0x10, poped_from_buffer);

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_push_byte_buffer_full_test()
{
    uint8_t input_array[]= {11,22,33,44,55,66,77,88};

    circ_bbuf_push_bytes(&buffer, input_array, buffer_size);
    TEST_ASSERT_EQUAL_UINT8(CBB_BUFFER_FULL, circ_bbuf_push(&buffer, 0x10));
}

void test_cbb_pop_byte_buffer_empty_test()
{
    uint8_t poped_from_buffer = 0x98;
    TEST_ASSERT_EQUAL_UINT8(CBB_BUFFER_EMPTY, circ_bbuf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(0x98, poped_from_buffer);
}

void test_cbb_push_bytes_to_full_array()
{
    uint8_t input_array[]= {10,20,30,40,50,60,70,80};

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_push_bytes(&buffer, input_array, buffer_size));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_full(&buffer));
}

void test_cbb_pop_bytes_to_empty_array()
{
    uint8_t input_array[]= {11,22,33,44,55,66,77,88};
    uint8_t popped_array[8];
    circ_bbuf_push_bytes(&buffer, input_array, buffer_size);

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_pop_bytes(&buffer, buffer_size, popped_array));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(input_array, popped_array, buffer_size);

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_push_bytes_circular_test()
{
    uint8_t input_array[]= {11,22,33,44,55,66,77,88};
    uint8_t input_array2[]= {15,26,37};
    uint8_t popped_array[8];

    circ_bbuf_push_bytes(&buffer, input_array, buffer_size);
    circ_bbuf_pop_bytes(&buffer, buffer_size, popped_array);

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_push_bytes(&buffer, input_array, sizeof(input_array2)));
    TEST_ASSERT_EQUAL_UINT32((buffer_size - sizeof(input_array2)), circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(sizeof(input_array2), circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_pop_bytes_circular_to_empty_array()
{
    uint8_t input_array[]= {11,22,33,44,55,66,77,88};
    uint8_t input_array2[]= {15,26,37};
    uint8_t popped_array[8];
    uint8_t popped_array2[3];

    circ_bbuf_push_bytes(&buffer, input_array, buffer_size);
    circ_bbuf_pop_bytes(&buffer, buffer_size, popped_array);
    circ_bbuf_push_bytes(&buffer, input_array2, sizeof(input_array2));


    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_pop_bytes(&buffer, sizeof(input_array2), popped_array2));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(input_array2, popped_array2, sizeof(input_array2));

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}