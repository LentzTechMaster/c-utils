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

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if resetting the buffer, after pushing data into it, makes it empty again
 */
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

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if buffer is empty
 */
void test_cbb_empty_buffer()
{
    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if after pushing one byte into the buffer, there is actually a byte in the buffer
 */
void test_cbb_insert_byte()
{
    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_push(&buffer, 0x10));
    TEST_ASSERT_EQUAL_UINT32(buffer_size -1, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(1, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if pushing a byte into the buffer then peeking it leaves it in the buffer; popping it makes the buffer empty again
 */
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

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if pushing a byte into the buffer then retrieving it with pop, correctly extracts the byte
 */
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

    //this seems a bit redundant with the previous test
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if buffer returns as full when pushing an array as big as the buffer in
 */
void test_cbb_push_byte_buffer_full_test()
{
    uint8_t input_array[]= {11,22,33,44,55,66,77,88};

    circ_bbuf_push_bytes(&buffer, input_array, buffer_size);
    TEST_ASSERT_EQUAL_UINT8(CBB_BUFFER_FULL, circ_bbuf_push(&buffer, 0x10));
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if buffer returns as empty when trying to pop out data that isn't there
 */
void test_cbb_pop_byte_buffer_empty_test()
{
    uint8_t poped_from_buffer = 0x98;
    TEST_ASSERT_EQUAL_UINT8(CBB_BUFFER_EMPTY, circ_bbuf_pop(&buffer, &poped_from_buffer));
    TEST_ASSERT_EQUAL_UINT8(0x98, poped_from_buffer);
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if buffer behaves correctly and returns as full when pushing a full size array in
 */
void test_cbb_push_bytes_to_full_array()
{
    uint8_t input_array[]= {10,20,30,40,50,60,70,80};

    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, circ_bbuf_push_bytes(&buffer, input_array, buffer_size));
    TEST_ASSERT_EQUAL_UINT32(0, circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(buffer_size, circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(TRUE, circ_bbuf_is_full(&buffer));

    //isn't this test redundant? test_cbb_push_byte_buffer_full_test
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if when pushing a full size array in, then popping it, the array is recovered correctly and the buffer is empty
 */
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

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if filling the buffer, emptying it, then inserting another array works correctly with the circular structure
 */
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


/**
 * @ingroup test_circular-byte-buffer
 * @brief check the case where after filling partially and then emptying the buffer, an array that wraps around with tail in front of head can be read
 */
void test_cbb_push_bytes_circular_test_wraparound()
{
    uint8_t input_array[]= {11,22,33,44,55};
    uint8_t input_array2[]= {15,26};
    uint8_t input_array3[]= {15,26,33,44,55};
    uint8_t popped_array[5];
    uint8_t popped_array2[2];
    uint8_t popped_array3[8];

    circ_bbuf_push_bytes(&buffer, input_array, sizeof(input_array));
    circ_bbuf_push_bytes(&buffer, input_array2, sizeof(input_array2));
    circ_bbuf_pop_bytes(&buffer, sizeof(popped_array), popped_array);
    uint8_t result = circ_bbuf_push_bytes(&buffer, input_array3, sizeof(input_array3));
    //try to fill the buffer and have the data wrap-around, with head before tail
    TEST_ASSERT_EQUAL_UINT8(CBB_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT32((buffer_size - sizeof(input_array3) - sizeof(input_array2)), circ_bbuf_available_space(&buffer));
    TEST_ASSERT_EQUAL_UINT32(sizeof(input_array3) + sizeof(input_array2), circ_bbuf_available_bytes_to_read(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_empty(&buffer));
    TEST_ASSERT_EQUAL_UINT8(FALSE, circ_bbuf_is_full(&buffer));
    
    //try to test when result isn't CBB_SUCCESS by popping too many bytes
    result = circ_bbuf_pop_bytes(&buffer, sizeof(popped_array3), popped_array3);
    TEST_ASSERT_EQUAL_UINT8(CBB_BUFFER_EMPTY, result);
}

/**
 * @ingroup test_circular-byte-buffer
 * @brief check if filling up the buffer twice, emptying it every time, works correctly
 */
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