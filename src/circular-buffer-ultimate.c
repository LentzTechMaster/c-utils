#include "circular-buffer-ultimate.h"

void circ_buf_init_buffer(circ_buf_t *buf, const uint32_t capacity, const uint32_t max_memory_size)
{
    circ_buf_t temp_buffer;
    temp_buffer.buffer = (circ_buf_elem_t *) malloc(capacity * sizeof(circ_buf_elem_t));
    temp_buffer.capacity = capacity;
    temp_buffer.max_memory_size = max_memory_size;
    temp_buffer.memory_size = 0;
    temp_buffer.buffer_status = CB_BUFFER_EMPTY;
    temp_buffer.head = 0;
    temp_buffer.tail = 0;

    *buf = temp_buffer;
}
void circ_buf_free_buffer(circ_buf_t *buf)
{
    for (uint32_t i = 0; i < circ_buf_available_elements_to_read(buf); i++)
    {
        free(buf->buffer[(buf->tail + i)%buf->capacity].pointer);
    }
    
    free(buf->buffer);
}

void circ_bbuf_reset_buffer(circ_buf_t *buf)
{
    circ_buf_free_buffer(buf);
    circ_buf_init_buffer(buf, buf->capacity, buf->max_memory_size);
}

uint32_t circ_buf_available_elements_to_read(circ_buf_t *buf)
{
    uint32_t elements_to_read = 0;

    if (buf->tail > buf->head)
    {
        elements_to_read = buf->capacity - buf->tail + buf->head;
    }else if (buf->head == buf->tail && buf->buffer_status == CB_BUFFER_FULL)
    {
        elements_to_read = buf->capacity;
    }else
    {
        elements_to_read = buf->head - buf->tail;
    }

    return elements_to_read;
}

uint32_t circ_buf_available_space(circ_buf_t *buf)
{
    return buf->capacity - circ_buf_available_elements_to_read(buf);
}

uint8_t circ_buf_is_empty(circ_buf_t *buf)
{
    return buf->buffer_status == CB_BUFFER_EMPTY;
}

uint8_t circ_buf_is_full(circ_buf_t *buf)
{
    return buf->buffer_status == CB_BUFFER_FULL;
}

uint8_t circ_buf_push_unsafe(circ_buf_t* buf, void* data, uint32_t data_size, uint32_t type)
{
    uint8_t result = CB_SUCCESS;

    if(!circ_buf_is_full(buf))
    {
        if(buf->max_memory_size == 0 | (buf->memory_size + data_size) <= buf->max_memory_size)
        {
            circ_buf_elem_t elem;
            elem.pointer = data;
            elem.size = data_size;
            elem.type = type;

            buf->memory_size += data_size;

            buf->buffer[buf->head++] = elem;
            buf->buffer_status = CB_BUFFER_FILLING;
            // Reset the head if reaching the size of the buffer
            if(buf->head >= buf->capacity) buf->head = 0;
        }
        else
        {
            result = CB_BUFFER_MAX_MEMORY_REACHED;
        }
    }
    else
    {
        result = CB_BUFFER_FULL;
    }

    // If head is joining the tail, all the buffer has been filled.
    if(buf->head == buf->tail && buf->buffer_status == CB_BUFFER_FILLING) buf->buffer_status = CB_BUFFER_FULL;

    return result;
}

uint8_t circ_buf_pop_unsafe(circ_buf_t *buf, circ_buf_elem_t *elem)
{
    uint8_t result = CB_SUCCESS;

    if(!circ_buf_is_empty(buf))
    {
        *elem = buf->buffer[buf->tail++];
        buf->buffer_status = CB_BUFFER_FILLING;
        // Reset the tail if reaching the size of the buffer
        if(buf->tail >= buf->capacity) buf->tail = 0;
    }else
    {
        result = CB_BUFFER_EMPTY;
    }
    
    // If tail is joining the head, all the buffer has been read.
    if(buf->head == buf->tail && buf->buffer_status == CB_BUFFER_FILLING) buf->buffer_status = CB_BUFFER_EMPTY;

    return result;
}

uint8_t circ_buf_push_safe(circ_buf_t *buf, void *data, uint32_t data_size, uint32_t type)
{
    void *data_to_push = (void*)malloc(data_size);
    memcpy(data_to_push, data, data_size);

    return circ_buf_push_unsafe(buf, data_to_push, data_size, type);
}


uint8_t circ_buf_peek_size_and_type(circ_buf_t *buf, uint32_t *size, uint32_t *type)
{
    uint8_t result = CB_SUCCESS;

    if(!circ_buf_is_empty(buf))
    {
        *size = buf->buffer[buf->tail].size;
        *type = buf->buffer[buf->tail].type;
    }else
    {
        result = CB_BUFFER_EMPTY;
    }

    return result;
}

uint8_t circ_buf_pop_safe(circ_buf_t *buf, void *data)
{
    uint8_t result;

    circ_buf_elem_t elem;

    result = circ_buf_pop_unsafe(buf, &elem);

    memcpy(data, elem.pointer, elem.size);
    free(elem.pointer);

    return result;
}
