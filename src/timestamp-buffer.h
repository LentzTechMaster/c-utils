/*
							 *******************
******************************* C HEADER FILE *******************************
**                           *******************                           **
**                                                                         **
** project   : c-utils                                                     ** 
** filename  : timestamp-buffer                                            **
** version   : 1                                                           **
** date      : January 11, 2019                                            **
** author    : Julien Delvaux                                              **
** licence   : MIT                                                         **
**                                                                         **
*****************************************************************************

The library goal is to timestamp incoming data whatever the medium is.
It is intended to be used with circular-byte-buffer library to store data.

*/

#ifndef TIMESTAMP_BUFFER_H_
#define TIMESTAMP_BUFFER_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct {
	uint64_t timestamp;
	uint32_t position;
	uint32_t length;
} timestamp_t;

typedef struct {
    timestamp_t * buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    uint8_t buffer_status;
} timestamp_buf_t;

enum TB_RESULT{TB_SUCCESS, TB_BUFFER_EMPTY, TB_BUFFER_FULL, TB_BUFFER_FILLING};

#define TMSTP_BUF_DEF(x,y)                \
    timestamp_t x##_data_space[y];        \
    timestamp_buf_t x = {                 \
        .buffer = x##_data_space,         \
        .head = 0,                        \
        .tail = 0,                        \
        .capacity = y,                    \
        .buffer_status = TB_BUFFER_EMPTY  \
    }

/**
* Create the circ_bbuf_t buffer inside micro controller environment
* @param buf
* @param size
* @return none
*/
void tstp_create_buffer(timestamp_buf_t *buf, const uint32_t size);

/**
* Gives how much free space is available in the buffer
* @param c
* @return Number of bytes that can be put into the buffer
*/
uint32_t tstp_available_space(timestamp_buf_t *c);

/**
* Gives how many bytes have not yet been read from the buffer
* @param c
* @return Number of bytes to read in the buffer
*/
uint32_t tstp_available_to_read(timestamp_buf_t *c);

/**
 * Check if circular buffer is empty
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t tstp_buf_is_empty(timestamp_buf_t *c);

/**
 * Check if circular buffer is full
 * @param c
 * @return FALSE
 *         TRUE
 */
uint8_t tstp_buf_is_full(timestamp_buf_t *c);

/**
 * Retrieve one timestamp from buffer
 * @param c
 * @param data
 * @return TB_SUCCESS
 *         TB_BUFFER_EMPTY
 */
uint8_t tstp_buf_pop(timestamp_buf_t *c, timestamp_t *data);


/**
 * Insert one timestamp in buffer
 * @param c
 * @param data
 * @return TB_SUCCESS
 *         TB_BUFFER_FULL
 */
uint8_t tstp_buf_push(timestamp_buf_t *c, timestamp_t *data);

#endif /* TIMESTAMP_BUFFER_H_ */