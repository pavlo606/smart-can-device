#include "App.hpp"
#include "main.h"
#include "usart.h"
#include <cstring>
#include <cstdlib>

#define PERIOD 5000

uint8_t uart1_rx;
uint8_t uart2_rx;

App app;

uint32_t lastGpsRequest;
uint8_t n = 0;

void App::init()
{
    sim7000.init();
    // obd2.init();
    led.init(GPIOC, GPIO_PIN_13);
    lastSend = 0;
    
    HAL_UART_Transmit(&huart2, (uint8_t*)"Fucking starting\n", 17, 100);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    // sim7000.sendCommand("AT+CFUN=1,1\r\n", CommandType::None);
    // sim7000.sendAT("AT\r\n");

    lastGpsRequest = HAL_GetTick();

    // HAL_UART_Receive_IT(&huart1, &uart1_rx, 1);
    // HAL_UART_Receive_IT(&huart2, &uart2_rx, 1);
}

void App::loop()
{
    sim7000.process();
    sim7000.stateProcess();

    uint32_t now = HAL_GetTick();



    if (now - lastGpsRequest > 12000)
    {
        // sim7000.sendCommand("AT+CGMR\r\n", CommandType::None);
        // sim7000.requestGNSS();
        sim7000.requestSendTelemetry("");
        // if (n == 0) {
        //     sim7000.sendAT("AT+CGDCONT=1,\"IP\",\"internet\"\r\n");
        // } else if (n == 1) {
        //     sim7000.sendAT("AT+CSTT=\"internet\"\r\n");
        // } else if (n == 2) {
        //     sim7000.sendAT("AT+CIICR\r\n");
        // } else if (n == 3) {
        //     sim7000.sendAT("AT+CIFSR");
        // } else if (n == 4) {
        //     sim7000.sendAT("AT+CIPSTART=\"TCP\",\"example.com\",\"80\"\r\n");
        // } else if (n == 5) {
        //     sim7000.sendAT("AT+CIPSEND\r\n");
        // } else if (n == 6) {
        //     sim7000.sendAT("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n\x1A");
        // } else if (n == 7) {
        //     sim7000.sendAT("AT+CIPCLOSE");
        // } else {
        //     n = 2;
        // }

        n++;

        lastGpsRequest = now;
    }
    // obd2.process();

    // uint32_t now = HAL_GetTick();

    // if (now - lastSend > PERIOD) {
    // //     auto data = telemetry.collect();
    // //     sim7000.send(data.toJson());

    //     lastSend = now;
    // }
    // led.set(GPIO_PIN_SET);
    // HAL_Delay(now - lastSend);
    
    // led.set(GPIO_PIN_RESET);
    // HAL_Delay(PERIOD - now - lastSend);
    
    // led.set(GPIO_PIN_SET);
    // HAL_Delay(PERIOD);
    
    // led.set(GPIO_PIN_RESET);
    // HAL_Delay(PERIOD);

}

Sim7000& App::getSim7000()
{
    return sim7000;
}


// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//     if (huart->Instance == USART1)
//     {
//         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
//         // Дані з SIM7000E → ПК
//         HAL_UART_Transmit(&huart2, &uart1_rx, 1, HAL_MAX_DELAY);

//         // Перезапуск прийому
//         HAL_UART_Receive_IT(&huart1, &uart1_rx, 1);
//     }
//     else if (huart->Instance == USART2)
//     {
//         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
//         // Дані з ПК → SIM7000E
//         HAL_UART_Transmit(&huart1, &uart2_rx, 1, HAL_MAX_DELAY);

//         // Перезапуск прийому
//         HAL_UART_Receive_IT(&huart2, &uart2_rx, 1);
//     }
// }