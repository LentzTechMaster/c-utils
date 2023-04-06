/*
This library can help you store any type of data inside a circular buffer but data has to be always the same byte length.

/!\ the circular buffer overwrite if full has to be done by user by getting status of the push function,
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
enum CBF_RESULT{CBF_SUCCESS, CBF_BUFFER_EMPTY, CBF_BUFFER_FULL, CBF_BUFFER_FILLING};

/**
 * @brief Initiate the buffer and alocate the necessary memory depending on the capacity and the element_size given by user
 * 
 * @param buf Buffer pointer.
 * @param capacity Number of elements inside the buffer
 * @param element_size Size of one element that will be stored inside buffer.
 */
void circ_buf_flex_init_buffer(circ_buf_flex_t *buf, const uint32_t capacity, const uint16_t element_size);

/**
 * @brief Free the memory alocated inside the init.
 * 
 * @param buf Buffer pointer.
 */
void circ_buf_flex_free(circ_buf_flex_t *buf);

/**
 * @brief Frees buffer then initiate it.
 * 
 * @param buf Buffer pointer.
 */
void circ_buf_flex_reset(circ_buf_flex_t *buf);

/**
 * @brief Gets the number of elements inside the buffer
 * 
 * @param buf Buffer pointer.
 * @return uint32_t Number of elements in the buffer
 */
uint32_t circ_buf_flex_available_elements_to_read(circ_buf_flex_t *buf);

/**
 * @brief Returns us the available space of a buffer.
 * 
 * @param buf Buffer pointer.
 * @return uint32_t Number of element that could be added to the buffer.
 */
uint32_t circ_buf_flex_available_space(circ_buf_flex_t *buf);

/**
 * @brief Returns if the buffer is empty.
 * 
 * @param buf Buffer pointer.
 * @return uint8_t Boolean
 */
uint8_t circ_buf_flex_is_empty(circ_buf_flex_t *buf);

/**
 * @brief Returns if the buffer is full.
 * 
* @param buf Buffer pointer.
 * @return uint8_t Boolean
 */
uint8_t circ_buf_flex_is_full(circ_buf_flex_t *buf);

/**
 * @brief This pushes the data inside the buffer.
 * 
 * @param buf Buffer pointer.
 * @param data Pointer to data.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_flex_push(circ_buf_flex_t* buf, void* data);

/**
 * @brief This makes a copy of the data inside the buffer to the given data pointer.
 * 
 * @param buf Buffer pointer.
 * @param data Pointer to data.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_flex_pop(circ_buf_flex_t *buf, void* data);