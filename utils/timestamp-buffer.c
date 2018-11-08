#include "timestamp-buffer.h"

uint32_t tstp_available_space(timestamp_buf_t *c)
{
    return c->capacity - tstp_available_to_read(c);
}

uint32_t tstp_available_to_read(timestamp_buf_t *c)
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
	// If head is joining the tail, all the buffer has been filled.
	if(c->head == c->tail && c->buffer_status == TB_BUFFER_FILLING) c->buffer_status = TB_BUFFER_FULL;
	else if(c->head != c->tail) c->buffer_status = TB_BUFFER_FILLING;

	if(!tstp_buf_is_full(c))
	{
		c->buffer[c->head].timestamp = data->timestamp;
		c->buffer[c->head++].position = data->position;
		// Reset the head if reaching the size of the buffer
		if(c->head >= c->capacity) c->head = 0;
	}

	return c->buffer_status == TB_BUFFER_FULL;
}

uint8_t tstp_buf_pop(timestamp_buf_t *c, timestamp_t *data)
{
	// If tail is joining the head, all the buffer has been read.
	if(c->head == c->tail && c->buffer_status == TB_BUFFER_FILLING) c->buffer_status = TB_BUFFER_EMPTY;
	else if(c->head != c->tail) c->buffer_status = TB_BUFFER_FILLING;

	if(!tstp_buf_is_empty(c))
	{
		data->position = c->buffer[c->tail].position;
		data->timestamp = c->buffer[c->tail++].timestamp;
		// Reset the tail if reaching the size of the buffer
		if(c->tail >= c->capacity) c->tail = 0;
	}

	return c->buffer_status == TB_BUFFER_EMPTY;
}