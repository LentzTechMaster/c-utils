#define UNITY_LONG_WIDTH 64

#include "unity.h"
#include "timestamp-buffer.h"

static timestamp_buf_t buffer;
static const uint32_t buffer_size = 8;
static const uint8_t FALSE = 0;
static const uint8_t TRUE = 1;

void setUp(void)
{
    tstp_create_buffer(&buffer, buffer_size);
}

void tearDown(void)
{
}

void test_cbb_empty_buffer()
{
    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

void test_cbb_insert_byte()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    TEST_ASSERT_EQUAL_UINT8(TB_SUCCESS, tstp_buf_push(&buffer, &timestamp));
    TEST_ASSERT_EQUAL_UINT32(buffer_size -1, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(1, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

void test_cbb_read_byte()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    timestamp_t poped_from_buffer;
    tstp_buf_push(&buffer, &timestamp);

    TEST_ASSERT_EQUAL_UINT8(TB_SUCCESS, tstp_buf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT64(timestamp.timestamp, poped_from_buffer.timestamp);
    TEST_ASSERT_EQUAL_UINT32(timestamp.position, poped_from_buffer.position);
    TEST_ASSERT_EQUAL_UINT32(timestamp.length, poped_from_buffer.length);

    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

void test_cbb_pop_byte_buffer_empty_test()
{
    timestamp_t poped_from_buffer;
    TEST_ASSERT_EQUAL_UINT8(TB_BUFFER_EMPTY, tstp_buf_pop(&buffer, &poped_from_buffer));
}