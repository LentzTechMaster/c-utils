#include <stdint.h>
#include <stdlib.h>

#ifdef CEEDLING_WARNING_MEMCPY
#include <string.h>
#endif

typedef struct {
    void * pointer;
    uint32_t size;
    uint32_t type;
} circ_buf_elem_t;

typedef struct {
    circ_buf_elem_t * buffer;
    uint32_t head;
    uint32_t tail;
    uint32_t max_memory_size;
    uint32_t memory_size;
    uint32_t capacity;
    uint8_t buffer_status;
} circ_buf_t;



enum CB_RESULT{CB_SUCCESS, CB_BUFFER_EMPTY, CB_BUFFER_FULL, CB_BUFFER_FILLING, CB_BUFFER_MAX_MEMORY_REACHED};

void circ_buf_init_buffer(circ_buf_t *buf, const uint32_t capacity, const uint32_t max_memory_size);

void circ_buf_free_buffer(circ_buf_t *buf);

void circ_bbuf_reset_buffer(circ_buf_t *buf);

uint32_t circ_buf_available_elements_to_read(circ_buf_t *buf);

uint32_t circ_buf_available_space(circ_buf_t *buf);

uint8_t circ_buf_is_empty(circ_buf_t *buf);

uint8_t circ_buf_is_full(circ_buf_t *buf);

uint8_t circ_buf_push_unsafe(circ_buf_t *buf, void *data, uint32_t data_size, uint32_t type);

uint8_t circ_buf_pop_unsafe(circ_buf_t *buf, circ_buf_elem_t *elem);

uint8_t circ_buf_peek_size_and_type(circ_buf_t *buf, uint32_t *size, uint32_t *type);

uint8_t circ_buf_push_safe(circ_buf_t *buf, void *data, uint32_t data_size, uint32_t type);

uint8_t circ_buf_pop_safe(circ_buf_t *buf, void *data);

