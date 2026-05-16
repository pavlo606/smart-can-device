#include "Led.hpp"

void Led::init(GPIO_TypeDef* port, uint16_t pin) {
  this->port_ = port;
  this->pin_ = pin;
}

void Led::set(GPIO_PinState state) {
  HAL_GPIO_WritePin(port_, pin_, state);
}

void Led::toggle() {
  HAL_GPIO_TogglePin(port_, pin_);
}