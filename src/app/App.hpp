#pragma once

#include "stm32f1xx_hal.h"
#include <services/led/Led.hpp>
#include <services/sim7000/Sim7000.hpp>
// #include "../services/sim7000/Sim7000.hpp"
// #include "../services/obd2/Obd2Service.hpp"
// #include "../services/telemetry/TelemetryService.hpp"

class App {
public:
    void init();
    void loop();

    Sim7000& getSim7000();

private:
    Sim7000 sim7000;
    // Obd2Service obd2;
    // TelemetryService telemetry;
    Led led;
    uint32_t lastSend = 0;
};