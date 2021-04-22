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

/**
 * @ingroup test_timestamp-buffer
 * @brief check if resetting the buffer, after pushing data into it, makes it empty again
 */
void test_tstp_reset_buffer()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    
    tstp_buf_push(&buffer, &timestamp);

    tstp_reset_buffer(&buffer);
    
    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

/**
 * @ingroup test_timestamp-buffer
 * @brief check that when a buffer is initialized, it is empty
 */
void test_tstp_empty_buffer()
{
    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

/**
 * @ingroup test_timestamp-buffer
 * @brief check if when a single timestamp is pushed into buffer, it is correctly registered
 */
void test_tstp_insert_byte()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    TEST_ASSERT_EQUAL_UINT8(TB_SUCCESS, tstp_buf_push(&buffer, &timestamp));
    TEST_ASSERT_EQUAL_UINT32(buffer_size -1, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(1, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}

/**
 * @ingroup test_timestamp-buffer
 * @brief check if a timestamp element can be inserted then extracted, and if that empties the buffer
 */
void test_tstp_read_byte()
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

/**
 * @ingroup test_timestamp-buffer
 * @brief check if for a request to extract a byte from an empty buffer, the buffer returns an empty result
 */
void test_tstp_pop_byte_buffer_empty_test()
{
    timestamp_t poped_from_buffer;
    TEST_ASSERT_EQUAL_UINT8(TB_BUFFER_EMPTY, tstp_buf_pop(&buffer, &poped_from_buffer));
}

/**
 * @ingroup test_timestamp-buffer
 * @brief check the case where after filling partially and then emptying the buffer, an array that wraps around with tail in front of head can be read
 */
void test_tstp_push_bytes_circular_test_wraparound()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    timestamp_t timestamp2 = {.timestamp = 1547202787926, .position = 15, .length =10};
    uint8_t i;

    for(i=0;i<6;i++){
        tstp_buf_push(&buffer, &timestamp);
    }
    TEST_ASSERT_EQUAL_UINT32((buffer_size - 6), tstp_available_space(&buffer));
    
    timestamp_t popped_from_buffer;

    for(i=0;i<6;i++){
        tstp_buf_pop(&buffer, &popped_from_buffer);
    }

    for(i=0;i<5;i++){
        tstp_buf_push(&buffer, &timestamp);
    }

    uint8_t result = tstp_buf_push(&buffer, &timestamp2);
    //try to fill the buffer and have the data wrap-around, with head before tail
    TEST_ASSERT_EQUAL_UINT8(TB_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT32((buffer_size - 6), tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(6, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
    
    //try to test when result isn't CBB_SUCCESS by popping too many bytes
    for(i=0;i<6;i++){
        tstp_buf_pop(&buffer, &popped_from_buffer);
    }

    result = tstp_buf_pop(&buffer, &popped_from_buffer);

    TEST_ASSERT_EQUAL_UINT8(TB_BUFFER_EMPTY, result);
}

/**
 * @ingroup test_timestamp-buffer
 * @brief check the case of filling the buffer entirely then emptying it
 */
void test_tstp_push_bytes_full_buffer_test()
{
    timestamp_t timestamp = {.timestamp = 1547202787926, .position = 15, .length =10};
    uint8_t i;

    for(i=0;i<8;i++){
        tstp_buf_push(&buffer, &timestamp);
    }
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_to_read(&buffer));
    
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_full(&buffer));

    uint8_t result = tstp_buf_push(&buffer, &timestamp);

    TEST_ASSERT_EQUAL_UINT8(TB_BUFFER_FULL, result);

    timestamp_t popped_from_buffer;

    for(i=0;i<8;i++){
        tstp_buf_pop(&buffer, &popped_from_buffer);
    }
    
    TEST_ASSERT_EQUAL_UINT32(buffer_size, tstp_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, tstp_available_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, tstp_buf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, tstp_buf_is_full(&buffer));
}