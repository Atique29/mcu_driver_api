#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

//pin state
//adding DRV_ to avoid name collisions with HAL 
typedef enum {
    DRV_GPIO_PIN_RESET = 0, 
    DRV_GPIO_PIN_SET = 1   
} gpio_pin_state_t;

//pin mode
typedef enum {
    DRV_GPIO_MODE_INPUT,                
    DRV_GPIO_MODE_OUTPUT_PP,            
    DRV_GPIO_MODE_OUTPUT_OD,            
    DRV_GPIO_MODE_ANALOG,               
    DRV_GPIO_MODE_IT_RISING,            
    DRV_GPIO_MODE_IT_FALLING,           
    DRV_GPIO_MODE_IT_RISING_FALLING     
} gpio_mode_t;

//internal pull-up/pull-down resistor
typedef enum {
    DRV_GPIO_PULL_NO,       
    DRV_GPIO_PULL_UP,       
    DRV_GPIO_PULL_DOWN      
} gpio_pull_t;

//gpio port
typedef void* gpio_port_t; 

//function pointer type for GPIO interrupt callbacks
typedef void (*gpio_callback_t)(uint8_t pin_number);

//config struct for initialization
typedef struct {
    gpio_port_t port;         
    uint8_t     pin_number;   
    gpio_mode_t mode;         
    gpio_pull_t pull;         
} gpio_config_t;


//user api functions
void gpio_init(const gpio_config_t* config);
void gpio_deinit(gpio_port_t port, uint8_t pin_number);

//gpio output
void gpio_set(gpio_port_t port, uint8_t pin_number);
void gpio_reset(gpio_port_t port, uint8_t pin_number);
void gpio_toggle(gpio_port_t port, uint8_t pin_number);

//gpio input
gpio_pin_state_t gpio_read(gpio_port_t port, uint8_t pin_number);

//user-defined callback function for a specific interrupt pin
void gpio_register_callback(uint8_t pin_number, gpio_callback_t callback);

#endif // GPIO_DRIVER_H
