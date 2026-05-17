#pragma once

#include "stm32f1xx_hal.h"
#include <services/led/Led.hpp>
#include <services/sim7000/Sim7000.hpp>
// #include "../services/sim7000/Sim7000.hpp"
// #include "../services/obd2/Obd2Service.hpp"
// #include "../services/telemetry/TelemetryService.hpp"

extern "C" {
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart);
}

class App {
public:
    void init();
    void loop();

    Sim7000& getSim7000();

private:
    friend void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
    friend void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

private:
    Sim7000 sim7000;
    // Obd2Service obd2;
    // TelemetryService telemetry;
    Led led;
    uint32_t lastSend = 0;
};