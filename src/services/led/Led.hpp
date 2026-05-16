#pragma once

#include "stm32f1xx_hal.h"

class Led {
private:
      GPIO_TypeDef* port_;
      uint16_t pin_;

public:
    void init(GPIO_TypeDef* port, uint16_t pin);
    void toggle();
    void set(GPIO_PinState state);
};