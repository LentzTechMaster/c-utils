#include "circular-buffer-flex.h"

void circ_buf_flex_init_buffer(circ_buf_flex_t *buf, const uint32_t capacity, const uint16_t element_size)
{
    circ_buf_flex_t temp_buffer;
    temp_buffer.buffer = (void *) malloc(capacity * element_size);
    temp_buffer.buffer_element_size = element_size;
    temp_buffer.capacity = capacity;
    temp_buffer.buffer_status = CBF_BUFFER_EMPTY;
    temp_buffer.head = 0;
    temp_buffer.tail = 0;

    *buf = temp_buffer;
}
void circ_buf_flex_free(circ_buf_flex_t *buf)
{   
    free(buf->buffer);
}

void circ_buf_flex_reset(circ_buf_flex_t *buf)
{
    buf->tail = 0;
    buf->head = 0;
    buf->buffer_status = CBF_BUFFER_EMPTY;
}

uint32_t circ_buf_flex_available_elements_to_read(circ_buf_flex_t *buf)
{
    uint32_t elements_to_read = 0;

    if (buf->tail > buf->head)
    {
        elements_to_read = buf->capacity - buf->tail + buf->head;
    }else if (buf->head == buf->tail && buf->buffer_status == CBF_BUFFER_FULL)
    {
        elements_to_read = buf->capacity;
    }else
    {
        elements_to_read = buf->head - buf->tail;
    }

    return elements_to_read;
}

uint32_t circ_buf_flex_available_space(circ_buf_flex_t *buf)
{
    return buf->capacity - circ_buf_flex_available_elements_to_read(buf);
}

uint8_t circ_buf_flex_is_empty(circ_buf_flex_t *buf)
{
    return buf->buffer_status == CBF_BUFFER_EMPTY;
}

uint8_t circ_buf_flex_is_full(circ_buf_flex_t *buf)
{
    return buf->buffer_status == CBF_BUFFER_FULL;
}

uint8_t circ_buf_flex_push(circ_buf_flex_t* buf, void* data)
{
    uint8_t result = CBF_SUCCESS;

    if(!circ_buf_flex_is_full(buf))
    {
        memcpy(((uint8_t*)buf->buffer) + (buf->head++) * (buf->buffer_element_size), data, buf->buffer_element_size);
        //buf->buffer[buf->head++] = *data;
        buf->buffer_status = CBF_BUFFER_FILLING;
        // Reset the head if reaching the size of the buffer
        if(buf->head >= buf->capacity) buf->head = 0;
    }
    else
    {
        result = CBF_BUFFER_FULL;
    }

    // If head is joining the tail, all the buffer has been filled.
    if(buf->head == buf->tail && buf->buffer_status == CBF_BUFFER_FILLING) buf->buffer_status = CBF_BUFFER_FULL;

    return result;
}

uint8_t circ_buf_flex_pop(circ_buf_flex_t *buf, void* data)
{
    uint8_t result = CBF_SUCCESS;

    if (!circ_buf_flex_is_empty(buf))
    {
        memcpy(data, ((uint8_t*)buf->buffer)+(buf->tail++)*(buf->buffer_element_size), buf->buffer_element_size);
        //memcpy((uint8_t*)data, buf->buffer[buf->tail], buf->buffer_element_size);
        buf->buffer_status = CBF_BUFFER_FILLING;
        
        // Reset the tail if reaching the size of the buffer
        if (buf->tail >= buf->capacity) buf->tail = 0;
    }
    else
    {
        result = CBF_BUFFER_EMPTY;
    }
    
    // If tail is joining the head, all the buffer has been read.
    if (buf->head == buf->tail && buf->buffer_status == CBF_BUFFER_FILLING) buf->buffer_status = CBF_BUFFER_EMPTY;

    return result;
}

uint8_t circ_buf_flex_drop(circ_buf_flex_t *buf)
{
    uint8_t result = CBF_SUCCESS;

    if (!circ_buf_flex_is_empty(buf))
    {
        buf->tail++;
        buf->buffer_status = CBF_BUFFER_FILLING;
        
        // Reset the tail if reaching the size of the buffer
        if (buf->tail >= buf->capacity) buf->tail = 0;
    }
    else
    {
        result = CBF_BUFFER_EMPTY;
    }
    
    // If tail is joining the head, all the buffer has been read.
    if (buf->head == buf->tail && buf->buffer_status == CBF_BUFFER_FILLING) buf->buffer_status = CBF_BUFFER_EMPTY;

    return result;
}
