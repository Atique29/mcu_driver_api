//backend implementation of gpio driver using the STM32 HAL library

#include "core/inc/config.h"
#ifdef USE_BACKEND_STM32_HAL

#include "drivers/inc/uart_driver.h"
#include "stm32f4xx_hal.h"
#include "stddef.h"

//HAL only calls HAL_UART_TxCpltCallback for interrupts on any uart
//need this struct for redirecting HALs callback to right uart
//struct to hold callback pointer and HALs handle info for one uart
typedef struct {
    UART_HandleTypeDef* hal_handle;
    uart_callback_t tx_complete_callback;
    uart_callback_t rx_complete_callback;
    uart_callback_t error_callback;
} uart_internal_handle_t;

//array to hold multiple uart internal handles
//maximum 5 instances, for stm32f446
//extendable for other mcu's
#define MAX_UART_HANDLES 5
static uart_internal_handle_t uart_handles[MAX_UART_HANDLES];
static uint8_t handle_count = 0;

//This function is used to access the callback function pointers in
//internal hanlde 
static uart_internal_handle_t* get_internal_handle(uart_handle_t handle){
    for (uint8_t i = 0; i < handle_count; i++) {
        if (uart_handles[i].hal_handle == (UART_HandleTypeDef*) handle){
            return &uart_handles[i];
        }
    }
    return NULL;
}

//user api functions 
bool uart_init(const uart_config_t* config){
    //safety checks
    if (config == NULL){
        return false;
    }
    
    //abort if user trying to cross MAX_UART_HANDLES
    if (handle_count >= MAX_UART_HANDLES) {
        return false;
    }

    //convert to HAL handle type
    UART_HandleTypeDef* huart = (UART_HandleTypeDef*) config->handle;

    //setup hal config for uart
    huart->Instance = (USART_TypeDef*) config->hw_instance;
    

    huart->Init.BaudRate = config->baud_rate;

    huart->Init.WordLength = (config->word_length == DRV_UART_WORD_LENGTH_8B) ? UART_WORDLENGTH_8B : UART_WORDLENGTH_9B;
    huart->Init.StopBits = (config->stop_bits == DRV_UART_STOP_BITS_1B) ? UART_STOPBITS_1 : UART_STOPBITS_2;

    switch(config->parity_mode){
        case DRV_UART_PARITY_NONE: huart->Init.Parity = UART_PARITY_NONE; break;
        case DRV_UART_PARITY_EVEN: huart->Init.Parity = UART_PARITY_EVEN; break;
        case DRV_UART_PARITY_ODD: huart->Init.Parity = UART_PARITY_ODD; break;
    }

    //default values for parameters that were excluded from driver 
    //to keep things simple
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    //initialize uart
    // HAL_StatusTypeDef status = HAL_UART_Init(huart);
    // return (status == HAL_OK);
    if(HAL_UART_Init(huart) != HAL_OK)
    {
        return false;
    }

    //store intialized handle for later callbacks
    //callback function pointers stored as NULL and
    //will be used by callback registering functions
    uart_handles[handle_count].hal_handle = huart;
    uart_handles[handle_count].tx_complete_callback = NULL;
    uart_handles[handle_count].rx_complete_callback = NULL;
    uart_handles[handle_count].error_callback = NULL;
    handle_count++;

    //init success
    return true;
}

void uart_deinit(uart_handle_t handle){
    HAL_UART_DeInit((UART_HandleTypeDef*) handle);
}


bool uart_transmit_polling(uart_handle_t handle, uint8_t*  data_ptr, uint16_t size, uint32_t timeout_ms){
    HAL_StatusTypeDef status = HAL_UART_Transmit((UART_HandleTypeDef*) handle, data_ptr, size, timeout_ms);
    return (status == HAL_OK);
}

bool uart_receive_polling(uart_handle_t handle, uint8_t* data_ptr, uint16_t size, uint32_t timeout_ms){
    HAL_StatusTypeDef status = HAL_UART_Receive((UART_HandleTypeDef*) handle, data_ptr, size, timeout_ms);
    return (status == HAL_OK);
}

bool uart_transmit_it(uart_handle_t handle, uint8_t*  data_ptr, uint16_t size){
    HAL_StatusTypeDef status = HAL_UART_Transmit_IT((UART_HandleTypeDef*) handle, data_ptr, size);
    return (status == HAL_OK);
}

bool uart_receive_it(uart_handle_t handle, uint8_t* data_ptr, uint16_t size){
    HAL_StatusTypeDef status = HAL_UART_Receive_IT((UART_HandleTypeDef*) handle, data_ptr, size);
    return (status == HAL_OK);
}

void uart_tx_register_callback(uart_handle_t handle, uart_callback_t callback){
    uart_internal_handle_t* internal_handle = get_internal_handle(handle);
    if (internal_handle != NULL){
        internal_handle->tx_complete_callback = callback;
    }
}

void uart_rx_register_callback(uart_handle_t handle, uart_callback_t callback){
    uart_internal_handle_t* internal_handle = get_internal_handle(handle);
    if (internal_handle != NULL){
        internal_handle->rx_complete_callback = callback;
    }
}

//HAL weak functions redefined
//HAL calls this on finishing transmission
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    uart_internal_handle_t* internal_handle = get_internal_handle((uart_handle_t*) huart);
    if (internal_handle != NULL && internal_handle->tx_complete_callback != NULL){
        internal_handle->tx_complete_callback((uart_handle_t*) huart);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    uart_internal_handle_t* internal_handle = get_internal_handle((uart_handle_t*) huart);
    if (internal_handle != NULL && internal_handle->rx_complete_callback != NULL){
        internal_handle->rx_complete_callback((uart_handle_t*) huart);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    uart_internal_handle_t* internal_handle = get_internal_handle((uart_handle_t*) huart);
    if (internal_handle != NULL && internal_handle->error_callback != NULL) {
        internal_handle->error_callback((uart_handle_t*) huart);
    }
}

#endif
