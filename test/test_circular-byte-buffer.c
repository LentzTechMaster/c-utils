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

void test_cbb_read_byte()
{
    uint8_t poped_from_buffer;
    circ_bbuf_push(&buffer, 0x10);

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(poped_from_buffer, 0x10);

    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

void test_cbb_insert_bytes_to_full_array()
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