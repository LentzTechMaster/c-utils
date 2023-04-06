/*
This library can help you store any type of data inside a circular buffer.
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


/**
 * @brief Enumeration of all results possible when doing action with the circular buffer
 * 
 */
enum CB_RESULT{CB_SUCCESS, CB_BUFFER_EMPTY, CB_BUFFER_FULL, CB_BUFFER_FILLING, CB_BUFFER_MAX_MEMORY_REACHED};

/**
 * @brief Initiate the buffer and alocate the necessary memory depending on the capacity given by user
 * 
 * @param buf Buffer pointer.
 * @param capacity Number of elements inside the buffer
 * @param max_memory_size Max number of bytes the buffer can be. If set to 0 it is unlimited.
 */
void circ_buf_init_buffer(circ_buf_t *buf, const uint32_t capacity, const uint32_t max_memory_size);

/**
 * @brief Free memory alocated for each element content of the buffer and also the memory alocated inside the init. 
 * 
 * @param buf Buffer pointer.
 */
void circ_buf_free_buffer(circ_buf_t *buf);

/**
 * @brief Frees buffer then initiate it.
 * 
 * @param buf Buffer pointer.
 */
void circ_buf_reset_buffer(circ_buf_t *buf);

/**
 * @brief Gets the number of elements inside the buffer
 * 
 * @param buf Buffer pointer.
 * @return uint32_t Number of elements in the buffer
 */
uint32_t circ_buf_available_elements_to_read(circ_buf_t *buf);

/**
 * @brief Returns us the available space of a buffer.
 * 
 * @param buf Buffer pointer.
 * @return uint32_t Number of element that could be added to the buffer
 */
uint32_t circ_buf_available_space(circ_buf_t *buf);

/**
 * @brief Returns if the buffer is empty.
 * 
 * @param buf Buffer pointer.
 * @return uint8_t Boolean
 */
uint8_t circ_buf_is_empty(circ_buf_t *buf);

/**
 * @brief Returns if the buffer is full.
 * 
* @param buf Buffer pointer.
 * @return uint8_t Boolean
 */
uint8_t circ_buf_is_full(circ_buf_t *buf);

/**
 * @brief This pushes the data inside the buffer. /!\ This function not safe: user has to alocate memory himself and therefore free it!
 * 
 * @param buf Buffer pointer.
 * @param data Pointer to data.
 * @param data_size Size of data in bytes.
 * @param type Integer that represent the type of data stored. This is a chosen by the user. It is made to help user remember what the data is.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_push_unsafe(circ_buf_t *buf, void *data, uint32_t data_size, uint32_t type);

/**
 * @brief This give to the user a circular buffer element giving access to the data via a pointer. /!\ This function not safe: user has to alocate memory himself and therefore free it!
 * 
 * @param buf Buffer pointer.
 * @param elem Point to circular buffer element.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_pop_unsafe(circ_buf_t *buf, circ_buf_elem_t *elem);

/**
 * @brief This give the user a peek at what is the size and the type of the next data to pop. It is very usefull to have when dealing with safe poping.
 * 
 * @param buf Buffer pointer.
 * @param size Integer pointer to store the size.
 * @param type Integer pointer to store the type.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_peek_size_and_type(circ_buf_t *buf, uint32_t *size, uint32_t *type);

/**
 * @brief This pushes the data inside the buffer. This function is safe meaning it takes care of the memory interactions.
 * 
 * @param buf Buffer pointer.
 * @param data Pointer to data.
 * @param data_size Size of data in bytes.
 * @param type Integer that represent the type of data stored. This is a chosen by the user. It is made to help user remember what the data is.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_push_safe(circ_buf_t *buf, void *data, uint32_t data_size, uint32_t type);

/**
 * @brief This makes a copy of the data inside the buffer to the given data pointer. This function is safe meaning it takes care of the memory interactions.
 * 
 * @param buf Buffer pointer.
 * @param data Pointer to data.
 * @return uint8_t Action status.
 */
uint8_t circ_buf_pop_safe(circ_buf_t *buf, void *data);