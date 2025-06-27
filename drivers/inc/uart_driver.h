#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

//uart handle 
typedef void* uart_handle_t;

//function pointer type for uart callbacks
typedef void (*uart_callback_t) (uart_handle_t handle);

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
    uart_handle_t handle;
    uint32_t baud_rate;
    uart_wordlength_t word_length;
    uart_stopbits_t stop_bits ;
    uart_parity_t parity_mode ;
} uart_config_t;

//user api functions
//initiation
bool uart_init(const uart_config_t* config);
void uart_deinit(uart_handle_t handle);

//transmit-receive in polling mode
bool uart_transmit_polling(uart_handle_t handle, uint8_t*  data_ptr, uint16_t size, uint32_t timeout_ms);
bool uart_receive_polling(uart_handle_t handle, uint8_t* data_ptr, uint16_t size, uint32_t timeout_ms);

//transmit-receive in interrupt mode
bool uart_transmit_it(uart_handle_t handle, uint8_t*  data_ptr, uint16_t size);
bool uart_receive_it(uart_handle_t handle, uint8_t* data_ptr, uint16_t size);

//callback registering functions
void uart_tx_register_callback(uart_handle_t handle, uart_callback_t callback);
void uart_rx_register_callback(uart_handle_t handle, uart_callback_t callback);

#endif // UART_DRIVER_H
