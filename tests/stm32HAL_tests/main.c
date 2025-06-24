#include "stm32f4xx_hal.h"

//include the custom driver
#include "gpio_driver.h"

#define LED_PORT        GPIOA
#define LED_PIN         5

#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      13

//Private function prototypes
//void SystemClock_Config(void);
//static void MX_GPIO_Init(void);
void SystemClock_Config(void);
 
//interrupt callback function
void button_press_callback(uint8_t pin_number);

int main(void) {

    HAL_Init();
    SystemClock_Config();

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

    //register int. callback function
    gpio_register_callback(BUTTON_PIN, button_press_callback);

    while (1) {

    }
}

void button_press_callback(uint8_t pin_number) {
    if (pin_number == BUTTON_PIN) {

        // using driver's API to toggle the led's state
        gpio_toggle(LED_PORT, LED_PIN);

        //debouncing delay
        for(volatile int i = 0; i < 200000; i++);
    }
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

// You need to add this function if you don't have it elsewhere
void SysTick_Handler(void)
{
  HAL_IncTick();
}

