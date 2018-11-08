#include "circular-byte-buffer.h"


uint32_t circ_bbuf_available_space(circ_bbuf_t *c)
{
	return c->capacity - circ_bbuf_available_bytes_to_read(c);
}

uint32_t circ_bbuf_available_bytes_to_read(circ_bbuf_t *c)
{
    uint32_t bytes_to_read = 0;

    if (c->tail >= c->head)
    {
        bytes_to_read = c->capacity - c->tail + c->head;
    }else
    {
        bytes_to_read = c->head - c->tail;
    }

    return bytes_to_read;
}

uint8_t circ_bbuf_is_empty(circ_bbuf_t *c)
{
    return c->buffer_status == CBB_BUFFER_EMPTY;
}

uint8_t circ_bbuf_is_full(circ_bbuf_t *c)
{
    return c->buffer_status == CBB_BUFFER_FULL;
}

uint8_t circ_bbuf_push(circ_bbuf_t *c, uint8_t data)
{
    // If head is joining the tail, all the buffer has been filled.
    if(c->head == c->tail && c->buffer_status == CBB_BUFFER_FILLING) c->buffer_status = CBB_BUFFER_FULL;
    else if(c->head != c->tail) c->buffer_status = CBB_BUFFER_FILLING;

    if(!circ_bbuf_is_full(c))
    {
        c->buffer[c->head++] = data;
        // Reset the head if reaching the size of the buffer
        if(c->head >= c->capacity) c->head = 0;
    }

    return c->buffer_status == CBB_BUFFER_FULL;
}

uint8_t circ_bbuf_pop(circ_bbuf_t *c, uint8_t *data)
{
    // If tail is joining the head, all the buffer has been read.
    if(c->head == c->tail && c->buffer_status == CBB_BUFFER_FILLING) c->buffer_status = CBB_BUFFER_EMPTY;
    else if(c->head != c->tail) c->buffer_status = CBB_BUFFER_FILLING;

    if(!circ_bbuf_is_empty(c))
    {
        *data = c->buffer[c->tail++];
        // Reset the tail if reaching the size of the buffer
        if(c->tail >= c->capacity) c->tail = 0;
    }

    return c->buffer_status == CBB_BUFFER_EMPTY;
}

uint8_t circ_bbuf_push_bytes(circ_bbuf_t *c, const uint8_t *data, uint32_t len)
{
    uint8_t result = CBB_SUCCESS;

    for (uint32_t i = 0; i < len; ++i) {
        result = circ_bbuf_push(c, *data++);
        // If buffer is empty
        if(result != CBB_SUCCESS){
            break;
        }
    }

    return result;
}

uint8_t circ_bbuf_pop_bytes(circ_bbuf_t *c, uint32_t len, uint8_t *data)
{
    uint8_t result = CBB_SUCCESS;

    for (uint32_t i = 0; i < len; i++) {
        result = circ_bbuf_pop(c, data++);
        // If buffer full
        if(result != CBB_SUCCESS){
            break;
        }
    }

    return result;
}