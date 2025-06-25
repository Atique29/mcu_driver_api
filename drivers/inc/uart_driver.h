#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

//uart instance 
typedef void* uart_instance_t;

//function pointer type for uart callbacks
typedef void (*uart_callback_t) (uart_instance_t instance);

//number of databits in a frame
typedef enum {
    UART_WORD_LENGTH_8B,
    UART_WORD_LENGTH_9B
} uart_wordlength_t;

//number of stop bits
typedef enum {
    UART_STOP_BITS_1B,
    UART_STOP_BITS_2B
} uart_stopbits_t;

//parity mode
typedef enum {
    DRV_UART_PARITY_NONE,
    DRV_UART_PARITY_EVEN,
    DRV_UART_PARITY_ODD
} uart_parity_t;

//config struct for initialization
typedef struct {
    uart_instance_t instance;
    uint32_t baud_rate;
    uart_wordlength_t word_length;
    uart_stopbits_t stop_bits ;
    uart_parity_t parity_mode ;
} uart_config_t;

//user api functions

bool uart_init(const uart_config_t* config);
void uar_deinit(uart_instance_t instance);








































#endif // UART_DRIVER_H
