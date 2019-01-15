#include "timestamp-buffer.h"

void tstp_create_buffer(timestamp_buf_t *buf, const uint32_t size)
{
    timestamp_buf_t temp_buffer;
    temp_buffer.buffer = (timestamp_t *) malloc(size * sizeof(timestamp_t));
    temp_buffer.capacity = size;
    temp_buffer.buffer_status = TB_BUFFER_EMPTY;
    temp_buffer.head = 0;
    temp_buffer.tail = 0;

    *buf = temp_buffer;
}

uint32_t tstp_available_space(timestamp_buf_t *c)
{
    return c->capacity - tstp_available_to_read(c);
}

uint32_t tstp_available_to_read(timestamp_buf_t *c)
{
    uint32_t bytes_to_read = 0;

    if (c->tail > c->head)
    {
        bytes_to_read = c->capacity - c->tail + c->head;
    }else if (c->head == c->tail && c->buffer_status == TB_BUFFER_FULL)
    {
        bytes_to_read = c->capacity;
    }else
    {
        bytes_to_read = c->head - c->tail;
    }

    return bytes_to_read;
}

uint8_t tstp_buf_is_empty(timestamp_buf_t *c)
{
    return c->buffer_status == TB_BUFFER_EMPTY;
}

uint8_t tstp_buf_is_full(timestamp_buf_t *c)
{
	return c->buffer_status == TB_BUFFER_FULL;
}

uint8_t tstp_buf_push(timestamp_buf_t *c, timestamp_t *data)
{
	uint8_t result = TB_SUCCESS;

	if(!tstp_buf_is_full(c))
	{
		c->buffer[c->head].timestamp = data->timestamp;
		c->buffer[c->head].position = data->position;
		c->buffer[c->head++].length = data->length;
		c->buffer_status = TB_BUFFER_FILLING;
		// Reset the head if reaching the size of the buffer
		if(c->head >= c->capacity) c->head = 0;
	}else
    {
        result = TB_BUFFER_FULL;
    }

	if(c->head == c->tail && c->buffer_status == TB_BUFFER_FILLING) c->buffer_status = TB_BUFFER_FULL;

	return result;
}

uint8_t tstp_buf_pop(timestamp_buf_t *c, timestamp_t *data)
{
	uint8_t result = TB_SUCCESS;

	if(!tstp_buf_is_empty(c))
	{
		data->position = c->buffer[c->tail].position;
		data->timestamp = c->buffer[c->tail].timestamp;
		data->length = c->buffer[c->tail++].length;
		c->buffer_status = TB_BUFFER_FILLING;
		// Reset the tail if reaching the size of the buffer
		if(c->tail >= c->capacity) c->tail = 0;
	}
	else
    {
        result = TB_BUFFER_EMPTY;
    }

	if(c->head == c->tail && c->buffer_status == TB_BUFFER_FILLING) c->buffer_status = TB_BUFFER_EMPTY;

	return result;
}