//backend implementation of gpio driver using the STM32 HAL library

#include "config.h"
#ifdef USE_BACKEND_STM32_HAL

#include "gpio_driver.h"
#include "stm32f4xx_hal.h"


//store registered callback functions addr for each ext int
static callback_ptr_t callbacks[16] = {NULL};

//enable clock for the port
//I'm implementing for stm32f446, can be easily extended for others
static void enable_gpio_clock(gpio_port_t port) {
    if (port == GPIOA) {
            __HAL_RCC_GPIOA_CLK_ENABLE();
        } else if (port == GPIOB) {
            __HAL_RCC_GPIOB_CLK_ENABLE();
        } else if (port == GPIOC) {
            __HAL_RCC_GPIOC_CLK_ENABLE();
        } else if (port == GPIOD) {
            __HAL_RCC_GPIOD_CLK_ENABLE();
        } else if (port == GPIOE) {
            __HAL_RCC_GPIOE_CLK_ENABLE();
        } else if (port == GPIOF) {
            __HAL_RCC_GPIOF_CLK_ENABLE();
        } else if (port == GPIOG) {
            __HAL_RCC_GPIOG_CLK_ENABLE();
        } else if (port == GPIOH) {
            __HAL_RCC_GPIOH_CLK_ENABLE();
        }
}

//api function implementations
void gpio_init(const gpio_config_t* config){

    //safety check
    if (config ==  NULL || config->port == NULL){
        return;
    }

    enable_gpio_clock(config->port);

    //translation of config to stm32 HAL
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //mode
    switch (config->mode) {
        case DRV_GPIO_MODE_INPUT:
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            break;
        case DRV_GPIO_MODE_OUTPUT_PP:
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            break;
        case DRV_GPIO_MODE_OUTPUT_OD:
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
            break;
        case DRV_GPIO_MODE_ANALOG:
             GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            break;
        case DRV_GPIO_MODE_IT_RISING:
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
            break;
        case DRV_GPIO_MODE_IT_FALLING:
            GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
            break;
        case DRV_GPIO_MODE_IT_RISING_FALLING:
            GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
            break;
    }

    //pin
    GPIO_InitStruct.Pin = (1 << config->pin_number);

    //push-pull
    switch (config->pull) {
        case DRV_GPIO_PULL_NO:
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            break;
        case DRV_GPIO_PULL_UP:
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            break;
        case DRV_GPIO_PULL_DOWN:
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            break;
    }

    //setting default speed
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    //call the actual HAL function to initialize
    HAL_GPIO_Init((GPIO_TypeDef*) config->port, &GPIO_InitStruct);

    //set priority, enable interrupt
    if(config->mode >= DRV_GPIO_MODE_IT_RISING) { //all the higher entries of the mode enum are interruos
        IRQn_Type irq_num;

        if (config->pin_number < 5) {
            irq_num = EXTI0_IRQn + config->pin_number;

        } else if (config->pin_number < 10) {
            irq_num = EXTI9_5_IRQn;

        } else {
            irq_num = EXTI15_10_IRQn;
        }

        HAL_NVIC_SetPriority(irq_num, 5, 0); //setting a default priority for simplicity
        HAL_NVIC_EnableIRQ(irq_num);
    }
}

void gpio_deinit(gpio_port_t port, uint8_t pin_number) {
    HAL_GPIO_DeInit((GPIO_TypeDef*)port, (1 << pin_number));
}

void gpio_set(gpio_port_t port, uint8_t pin_number){
    HAL_GPIO_WritePin((GPIO_TypeDef*) port, (1 << pin_number), GPIO_PIN_SET);
}

void gpio_reset(gpio_port_t port, uint8_t pin_number) {
    HAL_GPIO_WritePin((GPIO_TypeDef*) port, (1 << pin_number), GPIO_PIN_RESET);
}

void gpio_toggle(gpio_port_t port, uint8_t pin_number) {
    HAL_GPIO_TogglePin((GPIO_TypeDef*) port, (1 << pin_number));
}

gpio_pin_state_t gpio_read(gpio_port_t port, uint8_t pin_number) {
    gpio_pin_state_t pin_state;
    pin_state = (gpio_pin_state_t) HAL_GPIO_ReadPin((GPIO_TypeDef*) port, (1 << pin_number));
    return pin_state;
}

void gpio_register_callback(uint8_t pin_number, callback_ptr_t callback_addr) {
    if (pin_number < 16) {
        //store the function pointer in table at pin index
        callbacks[pin_number] = callback_addr;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    //this function is called by HAL when ext ints happen
    //can't just do callbacks[GPIO_Pin] as GPIO_Pin is a bitmask, not int
    for (int pin=0 ; pin<16 ; pin++) {
        if (GPIO_Pin == (1 << pin)){
            if (callbacks[pin] != NULL){ //check for callbacks
                callbacks[pin](pin);
            }
        }
    }
}

#endif

