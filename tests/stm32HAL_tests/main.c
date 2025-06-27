/**
 * this code - 
 * 1. Blinks the led on PA5 using gpio_driver
 * 2. Echoes the sent character back to terminal using uart_driver
 * 3. "Button Pressed" is sent with uart when button on PC13 fires an interrupt
 *
 * other configuration for uart is done in stm32f4xx_hal_msp.c
 * interrupts are set up in stm32f4xx_it.c
*/

#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

//include the driver
#include "drivers/inc/gpio_driver.h"
#include "drivers/inc/uart_driver.h"

//pin definitions
#define LED_PORT        GPIOA
#define LED_PIN         5

#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      13

UART_HandleTypeDef huart2;
uint8_t rx_buffer[1]; //1 byte buffer to receive character

//interrupt callback functions
void button_press_handler(uint8_t pin_number);
void uart_rx_handler(uart_handle_t handle);

void SystemClock_Config(void);

int main(void) {

    HAL_Init();
    SystemClock_Config();

    //gpio setup
    //configure the LED pin
    gpio_config_t led_config = {
        .port = LED_PORT,
        .pin_number = LED_PIN,
        .mode = DRV_GPIO_MODE_OUTPUT_PP,
        .pull = DRV_GPIO_PULL_NO
    };
    gpio_init(&led_config);

    //configure the button pin
    gpio_config_t button_config = {
        .port = BUTTON_PORT,
        .pin_number = BUTTON_PIN,
        .mode = DRV_GPIO_MODE_IT_FALLING, //trigger interrupt on falling edge because
        .pull = DRV_GPIO_PULL_NO          //Nucleo board has an internal pull-up
    };
    gpio_init(&button_config);
    gpio_register_callback(BUTTON_PIN, button_press_handler);

    //uart setup 
    //using USART2 peripheral
    huart2.Instance = USART2;

    uart_config_t uart_config = {
        .handle = &huart2,
        .hw_instance = USART2,
        .baud_rate = 115200,
        .parity_mode = DRV_UART_PARITY_NONE,
        .word_length = DRV_UART_WORD_LENGTH_8B,
        .stop_bits = DRV_UART_STOP_BITS_1B,
    };
    uart_init(&uart_config);
    uart_rx_register_callback(&huart2, uart_rx_handler);
    //start receiving in interrupt mode
    uart_receive_it(&huart2, rx_buffer, 1);

    //main loop
    while (1) {
        gpio_toggle(LED_PORT, LED_PIN);
        HAL_Delay(500);
    }
}


void uart_rx_handler(uart_handle_t handle){
    //echo the char back
    uart_transmit_it(&huart2, rx_buffer, 1);
    //restart receiving
    uart_receive_it(&huart2, rx_buffer, 1);
}


void button_press_handler(uint8_t pin_number){
    //debounce delay 
    for (volatile int i = 0; i < 20000; i++);

    char* text = "Button pressed!\r\n";
    uart_transmit_polling(&huart2, (uint8_t*)text, strlen(text), 100);
}

//clock setup from stm32cubeide
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
