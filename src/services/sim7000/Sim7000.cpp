#include "Sim7000.h"

extern "C" {
#include "usart.h"
}

#include "cmsis_os.h"

#include <cstring>

#include <stdio.h>

//#include "printf.h"

void Sim7000::onDataReceived(UART_HandleTypeDef* huart, uint16_t len, Sim7000* inst)
{
    if (huart == &huart1) {
        inst->onRxEvent(len);
    } else if (huart == &huart2) {
        // inst->rxLength2 = len;
        // inst->isUart2ResponseReceived = true;
        // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, inst->rxBuffer2, sizeof(inst->rxBuffer2));
    }
}

void Sim7000::onErrorOccured(UART_HandleTypeDef* huart, Sim7000* inst)
{
    if (huart == &huart1) {
        HAL_UART_DMAStop(&huart1);
        HAL_UART_DeInit(&huart1);
        HAL_UART_Init(&huart1);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, inst->rxBuffer, sizeof(inst->rxBuffer));
    } else if (huart == &huart2) {
        // HAL_UART_DMAStop(&huart2);
        // HAL_UART_DeInit(&huart2);
        // HAL_UART_Init(&huart2);
        // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, inst->rxBuffer2, sizeof(inst->rxBuffer2));
    }
}

void Sim7000::init()
{
    // HAL_UART_Receive_DMA(&huart1, rxBuffer, RX_BUFFER_SIZE);

    // __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, sizeof(rxBuffer));
}

void Sim7000::startThread()
{
    init();

    //..

    for (;;) {

        process();
        stateProcess();

        osDelay(1);
    }
}

void Sim7000::process()
{
    if (!packetReady)
        return;

    packetReady = false;

    rxBuffer[rxLength] = '\0';

    parseLines((char*)rxBuffer);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxBuffer, sizeof(rxBuffer));
}

void Sim7000::stateProcess()
{
    uint32_t now = HAL_GetTick();

    switch (state) {
    case SimState::Idle:
        changeState(SimState::Init);
        break;

    case SimState::Init:
        sendCommand("AT\r\n", CommandType::AT);
        changeState(SimState::WaitAT);
        break;

    case SimState::WaitAT:
        if (responses.lastResponseOk) {
            changeState(SimState::CheckSIM);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::Error);
        }
        break;

    case SimState::CheckSIM:
        sendCommand("AT+CPIN?\r\n", CommandType::CPIN);
        changeState(SimState::WaitSIM);
        break;

    case SimState::WaitSIM:
        if (responses.lastResponseReady) {
            changeState(SimState::CheckNetwork);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::Error);
        }
        break;

    case SimState::CheckNetwork:
        sendCommand("AT+CREG?\r\n", CommandType::CREG);
        changeState(SimState::WaitNetwork);
        break;

    case SimState::WaitNetwork:
        if (responses.networkRegistered) {
            changeState(SimState::EnableGNSS);
        } else if (now - stateTimestamp > 5000) {
            changeState(SimState::Error);  // retry
        }
        break;

    case SimState::EnableGNSS:
        sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", CommandType::CGNSPWR);
        changeState(SimState::WaitGNSS);
        break;

    case SimState::WaitGNSS:
        if (responses.lastResponseOk) {
            changeState(SimState::SetupGPRS);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::Error);
        }
        break;

    case SimState::SetupGPRS:
        sendCommand("AT+SAPBR=3,1,\"APN\",\"internet\"\r\n", CommandType::CGATT);
        changeState(SimState::WaitGPRS);
        break;

    case SimState::WaitGPRS:
        if (responses.lastResponseOk) {
            changeState(SimState::SetupAPN);
        } else if (now - stateTimestamp > 5000) {
            changeState(SimState::Error);
        }
        break;

    case SimState::SetupAPN:
        sendCommand("AT+SAPBR=1,1\r\n", CommandType::CGDCONT);
        changeState(SimState::WaitAPN);
        break;

    case SimState::WaitAPN:
        if (responses.lastResponseOk) {
            changeState(SimState::CheckPDP);
        } else if (now - stateTimestamp > 1000) {
            changeState(SimState::CheckPDP);
        }
        break;

    case SimState::CheckPDP:
        sendCommand("AT+SAPBR=2,1\r\n", CommandType::CNACT);
        changeState(SimState::WaitCheckPDP);
        break;

    case SimState::WaitCheckPDP:
        if (responses.lastResponseOk) {
            changeState(SimState::Ready);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::Ready);
        }
        break;

    case SimState::ActivatePDP:
        sendCommand("AT+CIICR\r\n", CommandType::CNACT);
        changeState(SimState::WaitPDP);
        break;

    case SimState::WaitPDP:
        if (responses.lastResponseOk) {
            changeState(SimState::Ready);
        } else if (now - stateTimestamp > 5000) {
            changeState(SimState::Ready);
        }
        break;

    case SimState::Ready:
        // чекаємо send()
        break;

    case SimState::Sending:
        if (checkResponse("SEND OK")) {
            changeState(SimState::Ready);
        } else if (now - stateTimestamp > 5000) {
            changeState(SimState::Error);
        }
        break;

        // ----------- SEND TCP -------------

    case SimState::StartTCP:
        sendCommand("AT+CIPSTART=\"TCP\",\"example.com\",\"80\"\r\n", CommandType::CIPSTART);
        changeState(SimState::WaitStartTCP);
        break;

    case SimState::WaitStartTCP:
        if (responses.lastResponseOk) {
            changeState(SimState::SendTCP);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::CloseTCP);
        }
        break;

    case SimState::SendTCP:
        sendCommand("AT+CIPSEND\r\n", CommandType::CIPSEND);
        changeState(SimState::WaitSendTCP);
        break;

    case SimState::WaitSendTCP:
        if (responses.lastResponseOk) {
            changeState(SimState::SendDataTCP);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::CloseTCP);
        }
        break;

    case SimState::SendDataTCP:
        // sendCommand("AT+CIPSEND\r\n", CommandType::CIPSEND);
        sendAT("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n\x1A");
        changeState(SimState::WaitSendDataTCP);
        break;

    case SimState::WaitSendDataTCP:
        if (responses.lastResponseOk) {
            changeState(SimState::CloseTCP);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::CloseTCP);
        }
        break;

        // ----------- SEND HTTP ------------

    case SimState::HttpInit:
        sendCommand("AT+HTTPINIT\r\n", CommandType::HTTPINIT);
        changeState(SimState::WaitHttpInit);
        break;

    case SimState::WaitHttpInit:
        if (responses.lastResponseOk) {
            changeState(SimState::HttpSetCID);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::HttpTerminate);
        }
        break;

    case SimState::HttpSetCID:
        sendCommand("AT+HTTPPARA=\"CID\",1\r\n", CommandType::HTTPPARA);
        changeState(SimState::WaitHttpSetCID);
        break;

    case SimState::WaitHttpSetCID:
        if (responses.lastResponseOk) {
            changeState(SimState::HttpSetURL);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::HttpTerminate);
        }
        break;

    case SimState::HttpSetURL:
        sendCommand("AT+HTTPPARA=\"URL\",\"http://httpbin.org/get\"\r\n", CommandType::HTTPPARA);
        changeState(SimState::WaitHttpSetURL);
        break;

    case SimState::WaitHttpSetURL:
        if (responses.lastResponseOk) {
            changeState(SimState::HttpSend);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::HttpTerminate);
        }
        break;

    case SimState::HttpSend:
        sendCommand("AT+HTTPACTION=0\r\n", CommandType::HTTPACTION);
        changeState(SimState::WaitHttpSend);
        break;

    case SimState::WaitHttpSend:
        // if (responses.lastResponseOk) {
        //     changeState(SimState::HttpTerminate);
        // }
        if (now - stateTimestamp > 5000) {
            changeState(SimState::HttpRead);
        }
        break;

    case SimState::HttpRead:
        sendCommand("AT+HTTPREAD\r\n", CommandType::HTTPACTION);
        changeState(SimState::WaitHttpRead);
        break;

    case SimState::WaitHttpRead:
        // if (responses.lastResponseOk) {
        //     changeState(SimState::HttpTerminate);
        // }
        if (now - stateTimestamp > 2000) {
            changeState(SimState::HttpTerminate);
        }
        break;

    case SimState::HttpTerminate:
        sendCommand("AT+HTTPTERM\r\n", CommandType::HTTPTERM);
        changeState(SimState::WaitHttpTerminate);
        break;

    case SimState::WaitHttpTerminate:
        if (responses.lastResponseOk) {
            changeState(SimState::Ready);
        } else if (now - stateTimestamp > 2000) {
            changeState(SimState::Ready);
        }
        break;

        // -----------------------------------

    case SimState::Error:
        // retry after delay
        if (now - stateTimestamp > 3000) {
            changeState(SimState::Init);
            HAL_UART_Transmit(&huart2, (uint8_t*)"Error\n", 6, 100);
        }
        break;

    default:
        break;
    }

    resetResponses();
}

void Sim7000::sendAT(const char* cmd)
{ HAL_UART_Transmit(&huart1, (uint8_t*)cmd, strlen(cmd), 2000); }

void Sim7000::sendCommand(const char* cmd, CommandType type)
{
    currentCommand = type;

    sendAT(cmd);
}

void Sim7000::parseLines(char* data)
{
    char* line = strtok(data, "\r\n");

    while (line != nullptr) {
        handleLine(line);

        line = strtok(nullptr, "\r\n");
    }
}

void Sim7000::handleLine(const char* line)
{
    if (strlen(line) == 0)
        return;

    HAL_UART_Transmit(&huart2, (uint8_t*)"\nRecieved: \"", 12, 100);

    HAL_UART_Transmit(&huart2, (uint8_t*)line, strlen(line), 100);

    HAL_UART_Transmit(&huart2, (uint8_t*)"\"\n", 2, 100);

    printf("[SIM_7000] <<=== Received data: %s\r\n", line);

    if (strstr(line, "OK")) {
        responses.lastResponseOk = true;
    }

    if (strstr(line, "READY")) {
        responses.lastResponseReady = true;
    }

    if (currentCommand == CommandType::CREG) {
        if (strstr(line, "+CREG: 0,1")) {
            responses.networkRegistered = true;
        }
    }

    if (currentCommand == CommandType::CNACT) {
        if (strstr(line, "+CNACT: 1")) {
            responses.PDPActive = true;
        }
    }

    if (currentCommand == CommandType::CGNSINF) {
        if (strstr(line, "+CGNSINF:")) {
            gpsService.updateFromCGNSINF(line);
        }
    }
}

// ----------- Requests -------------

void Sim7000::requestGNSS()
{
    if (state != SimState::Ready)
        return;

    sendCommand("AT+CGNSINF\r\n", CommandType::CGNSINF);
}

void Sim7000::requestSendTelemetry(const char* json)
{
    if (state != SimState::Ready)
        return;

    strncpy(txPayload, json, sizeof(txPayload));

    changeState(SimState::HttpInit);
}

// ----------------------------------

void Sim7000::resetResponses()
{
    responses.lastResponseOk = false;
    responses.lastResponseReady = false;
    responses.networkRegistered = false;
    responses.PDPActive = false;
}

GpsService Sim7000::getGpsService()
{ return gpsService; }

bool Sim7000::checkResponse(const char* expected)
{
    if (strstr((char*)rxBuffer, expected)) {
        memset(rxBuffer, 0, sizeof(rxBuffer));
        return true;
    }
    return false;
}

void Sim7000::changeState(SimState newState)
{
    state = newState;
    stateTimestamp = HAL_GetTick();
    responseReceived = false;
}

uint8_t* Sim7000::getRxBuffer()
{ return rxBuffer; }

uint16_t Sim7000::getRxBufferSize()
{ return RX_BUFFER_SIZE; }

void Sim7000::onRxEvent(uint16_t len)
{
    rxLength = len;
    packetReady = true;
}
