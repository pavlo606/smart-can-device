#pragma once

// #include <services/led/Led.hpp>

// #include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "usart.h"

#include "Sim7000.h"
// #include "../services/sim7000/Sim7000.hpp"
// #include "../services/obd2/Obd2Service.hpp"
// #include "../services/telemetry/TelemetryService.hpp"

extern "C" {
extern void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
extern void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart);
}

class App
{
public:
    static void startLteThread();

    // void init();
    // void loop();

    // Sim7000& getSim7000();

private:
    friend void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
    friend void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

private:
    static Sim7000* s_sim7000;

private:
    // Sim7000 sim7000;
    // // Obd2Service obd2;
    // // TelemetryService telemetry;
    // Led led;
    // uint32_t lastSend = 0;
};