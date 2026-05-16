#pragma once
#include <stdint.h>
#include "services/gps/GpsService.hpp"
#include "Sim7000Responses.hpp"

enum class SimState
{
    Idle,
    Init,

    WaitAT,
    CheckSIM,
    WaitSIM,
    CheckNetwork,
    WaitNetwork,
    SetupGPRS,
    WaitGPRS,
    SetupAPN,
    WaitAPN,
    CheckPDP,
    WaitCheckPDP,
    ActivatePDP,
    WaitPDP,

    HttpInit,
    WaitHttpInit,
    HttpSetCID,
    WaitHttpSetCID,
    HttpSetURL,
    WaitHttpSetURL,
    HttpSetContent,
    WaitHttpSetContent,
    HttpSetData,
    WaitHttpSetData,
    HttpSend,
    WaitHttpSend,
    HttpRead,
    WaitHttpRead,
    HttpTerminate,
    WaitHttpTerminate,

    StartTCP,
    WaitStartTCP,
    SendTCP,
    WaitSendTCP,
    SendDataTCP,
    WaitSendDataTCP,
    CloseTCP,
    WaitCloseTCP,

    EnableGNSS,
    WaitGNSS,
    
    Ready,
    Sending,
    Error
};

enum class CommandType {
    None,
    AT,
    CFUN,
    CPIN,
    CREG,
    CGATT,
    CGNSPWR,
    CGNSINF,
    CGDCONT,
    CNACT,
    HTTPINIT,
    HTTPPARA,
    HTTPACTION,
    HTTPREAD,
    HTTPTERM,
    HTTPDATA,
    CIPSTART,
    CIPSEND,
    CIPCLOSE,
};

class Sim7000
{
public:
    static constexpr uint16_t RX_BUFFER_SIZE = 4096;
    static constexpr uint16_t TX_PAYLOAD_SIZE = 512;
    
    void init();
    void process();
    void stateProcess();

    void sendCommand(const char* cmd, CommandType type);
    
    void onRxEvent(uint16_t len);

    uint8_t* getRxBuffer();
    uint16_t getRxBufferSize();
    
    void requestGNSS();
    void requestSendTelemetry(const char* json);
    
    GpsService getGpsService();
    void sendAT(const char *cmd);
    
private:
    SimState state = SimState::Idle;
    uint32_t stateTimestamp = 0;
    uint8_t rxBuffer[RX_BUFFER_SIZE];
    bool responseReceived = false;
    
    volatile bool packetReady = false;
    volatile uint16_t rxLength = 0;

    char txPayload[TX_PAYLOAD_SIZE];
    
    
    void changeState(SimState newState);
    bool checkResponse(const char *expected);
    void parseLines(char* data);
    void handleLine(const char* line);
    
    CommandType currentCommand;

    Sim7000Responses responses;
    void resetResponses();

    GpsService gpsService;
};

// AT+CSQ: Check signal quality.
// AT+CPIN?: Verify SIM card readiness.
// AT+CGREG?: Check network registration.
// AT+CGDCONT=1,"IP","APN": Set PDP context (replace APN with carrier's APN).
// AT+CSTT="APN": Set APN, user name, and password.
// AT+CIICR: Bring up wireless connection (GPRS/LTE).
// AT+CIFSR: Get local IP address.
// AT+CIPSTART="TCP","IP","PORT": Start TCP connection.AT+CIPSEND: Send data.
// AT+CIPCLOSE: Close connection.
// AT+CIPSHUT: Reset IP session

// AT+CIPSEND=0,88
// GET / HTTP/1.1
// Host: smart-can-backend-production.up.railway.app
// Connection: close

// Host: example.com