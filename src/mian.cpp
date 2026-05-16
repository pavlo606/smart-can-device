#include "app/App.hpp"

extern "C" {
  #include "system_clock.h"
  #include "gpio.h"
  #include "can.h"
  #include "dma.h"
  #include "usart.h"
}

extern App app;
// App app;

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_CAN_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    HAL_Delay(250);

    app.init();

    // HAL_Delay(5000);

    while (true)
    {
      app.loop();
      HAL_Delay(1);
    }
}