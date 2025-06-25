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
    DRV_UART_WORD_LENGTH_8B,
    DRV_UART_WORD_LENGTH_9B
} uart_wordlength_t;

//number of stop bits
typedef enum {
    DRV_UART_STOP_BITS_1B,
    DRV_UART_STOP_BITS_2B
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
//initiation
bool uart_init(const uart_config_t* config);
void uart_deinit(uart_instance_t instance);

//transmit-receive in polling mode
bool uart_transmit_polling(uart_instance_t instance, uint8_t*  data_ptr, uint16_t size, uint32_t timeout_ms);
bool uart_receive_polling(uart_instance_t instance, uint8_t* data_ptr, uint16_t size, uint32_t timeout_ms);

//transmit-receive in interrupt mode
bool uart_transmit_it(uart_instance_t instance, const uint8_t*  data_ptr, uint16_t size);
bool uart_receive_it(uart_instance_t instance, uint8_t* data_ptr, uint16_t size);

//callback functions
void uart_tx_callback(uart_instance_t instance, uart_callback_t callback);
void uart_rx_callback(uart_instance_t instance, uart_callback_t callback);


#endif // UART_DRIVER_H
