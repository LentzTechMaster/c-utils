/*
							 *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : c-utils                                                     ** 
** filename  : circular-byte-buffer                                        **
** version   : 1                                                           **
** date      : January 11, 2019                                            **
** author    : Julien Delvaux                                              **
** licence   : MIT                                                         **
**                                                                         **
*****************************************************************************
Based on Siddharth Chandrasekaran library : https://github.com/EmbedJournal/c-utils

This implementation provides in addition to original library:
 - 3 more functions to check buffer status and availability
 - Push/Retrieve multiples bytes with one function call
 - Full buffer size can be used : every byte in buffer is used
 - Embedded micro-controller targeted
 
*/

#ifndef __CIRCULAR_BYTE_BUFFER_H_
#define __CIRCULAR_BYTE_BUFFER_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t * buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    uint8_t buffer_status;
} circ_bbuf_t;

enum CBB_RESULT{CBB_SUCCESS, CBB_BUFFER_EMPTY, CBB_BUFFER_FULL, CBB_BUFFER_FILLING};

// This has to be used outside of micro controllers
#define CIRC_BBUF_DEF(x,y)                 \
    uint8_t x##_data_space[y];             \
    circ_bbuf_t x = {                      \
        .buffer = x##_data_space,          \
        .head = 0,                         \
        .tail = 0,                         \
        .capacity = y,                     \
        .buffer_status = CBB_BUFFER_EMPTY  \
    }

/**
* Create the circ_bbuf_t buffer inside micro controller environment
* @param buf
* @param size
* @return none
*/
void circ_bbuf_create_buffer(circ_bbuf_t *buf, const uint32_t size);

/**
* Gives how much free space is available in the buffer
* @param c
* @return Number of bytes that can be put into the buffer
*/
uint32_t circ_bbuf_available_space(circ_bbuf_t *c);

/**
* Gives how many bytes have not yet been read from the buffer
* @param c
* @return Number of bytes to read in the buffer
*/
uint32_t circ_bbuf_available_bytes_to_read(circ_bbuf_t *c);

/**
 * Check if circular buffer is empty
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t circ_bbuf_is_empty(circ_bbuf_t *c);

/**
 * Check if circular buffer is full
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t circ_bbuf_is_full(circ_bbuf_t *c);

/**
 * Retrieve one byte from buffer
 * @param c
 * @param data
 * @return CBB_SUCCESS
 *         CBB_BUFFER_EMPTY
 */
uint8_t circ_bbuf_pop(circ_bbuf_t *c, uint8_t *data);


/**
 * Insert one byte in buffer
 * @param c
 * @param data
 * @return CBB_SUCCESS
 *         CBB_BUFFER_FULL
 */
uint8_t circ_bbuf_push(circ_bbuf_t *c, uint8_t data);

/**
 * Insert multiples bytes into buffer
 * @param c
 * @param data
 * @param len
 * @return CBB_SUCCESS
 *         CBB_BUFFER_FULL
 */
uint8_t circ_bbuf_push_bytes(circ_bbuf_t *c, const uint8_t *data, uint32_t len);

/**
 * Retrieve multiples bytes from buffer
 * @param c
 * @param data
 * @param len
 * @return CBB_SUCCESS
 *         CBB_BUFFER_FULL
 */
uint8_t circ_bbuf_pop_bytes(circ_bbuf_t *c, uint32_t len, uint8_t *data);

#endif /* __CIRCULAR_BYTE_BUFFER_H_ */
