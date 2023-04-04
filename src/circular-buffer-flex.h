/*
This library can help you store any type of data inside a circular buffer but it has to be this .
You have the choice to manage the memory your self by using usafe functions for pushing and poping operations.
Or you can use the safe functions and this is made for you.

You can setup a memory limit.

/!\ the circular buffer overwrite if full has to be done y user by getting status of the push function,
if it is not sucessful you can handle the poping of a element and then retry a push.

Check the tests to better understand how to use the lib.

Enjoy !!


███╗   ███╗ █████╗ ██████╗ ███████╗    ██╗    ██╗██╗████████╗██╗  ██╗    ██╗      ██████╗ ██╗   ██╗███████╗    ██████╗ ██╗   ██╗
████╗ ████║██╔══██╗██╔══██╗██╔════╝    ██║    ██║██║╚══██╔══╝██║  ██║    ██║     ██╔═══██╗██║   ██║██╔════╝    ██╔══██╗╚██╗ ██╔╝
██╔████╔██║███████║██║  ██║█████╗      ██║ █╗ ██║██║   ██║   ███████║    ██║     ██║   ██║██║   ██║█████╗      ██████╔╝ ╚████╔╝ 
██║╚██╔╝██║██╔══██║██║  ██║██╔══╝      ██║███╗██║██║   ██║   ██╔══██║    ██║     ██║   ██║╚██╗ ██╔╝██╔══╝      ██╔══██╗  ╚██╔╝  
██║ ╚═╝ ██║██║  ██║██████╔╝███████╗    ╚███╔███╔╝██║   ██║   ██║  ██║    ███████╗╚██████╔╝ ╚████╔╝ ███████╗    ██████╔╝   ██║   
╚═╝     ╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝     ╚══╝╚══╝ ╚═╝   ╚═╝   ╚═╝  ╚═╝    ╚══════╝ ╚═════╝   ╚═══╝  ╚══════╝    ╚═════╝    ╚═╝   
                                                                                                                                

██████╗ ███████╗███╗   ██╗     ██╗ █████╗ ███╗   ███╗██╗███╗   ██╗    ██╗     ███████╗███╗   ██╗████████╗███████╗
██╔══██╗██╔════╝████╗  ██║     ██║██╔══██╗████╗ ████║██║████╗  ██║    ██║     ██╔════╝████╗  ██║╚══██╔══╝╚══███╔╝
██████╔╝█████╗  ██╔██╗ ██║     ██║███████║██╔████╔██║██║██╔██╗ ██║    ██║     █████╗  ██╔██╗ ██║   ██║     ███╔╝ 
██╔══██╗██╔══╝  ██║╚██╗██║██   ██║██╔══██║██║╚██╔╝██║██║██║╚██╗██║    ██║     ██╔══╝  ██║╚██╗██║   ██║    ███╔╝  
██████╔╝███████╗██║ ╚████║╚█████╔╝██║  ██║██║ ╚═╝ ██║██║██║ ╚████║    ███████╗███████╗██║ ╚████║   ██║   ███████╗
╚═════╝ ╚══════╝╚═╝  ╚═══╝ ╚════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚═╝╚═╝  ╚═══╝    ╚══════╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝
                                                                                                                 
licence   : MIT 
*/


#include <string.h>//for memcpy

#if defined(TEST)
    #include <stdint.h>//for types
    #include <stdlib.h>//for malloc
#else
    #include "compiler.h"
#endif

typedef struct {
    void* buffer;
    uint16_t buffer_element_size;
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    uint8_t buffer_status;
} circ_buf_flex_t;

/**
 * @brief Enumeration of all results possible when doing action with the circular buffer
 * 
 */
enum CB_RESULT{CB_SUCCESS, CB_BUFFER_EMPTY, CB_BUFFER_FULL, CB_BUFFER_FILLING, CB_BUFFER_MAX_MEMORY_REACHED};

void circ_buf_flex_init_buffer(circ_buf_flex_t *buf, const uint32_t capacity, const uint16_t element_size);
void circ_buf_flex_free(circ_buf_flex_t *buf);

void circ_buf_flex_reset(circ_buf_flex_t *buf);

uint32_t circ_buf_flex_available_elements_to_read(circ_buf_flex_t *buf);

uint32_t circ_buf_flex_available_space(circ_buf_flex_t *buf);

uint8_t circ_buf_flex_is_empty(circ_buf_flex_t *buf);

uint8_t circ_buf_flex_is_full(circ_buf_flex_t *buf);

uint8_t circ_buf_flex_push(circ_buf_flex_t* buf, void* data);

uint8_t circ_buf_flex_pop(circ_buf_flex_t *buf, void* data);