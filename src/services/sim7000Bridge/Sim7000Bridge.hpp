#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sim7000_uart_rx_event(uint16_t len);

uint8_t* sim7000_get_rx_buffer(void);

uint16_t sim7000_get_rx_buffer_size(void);

#ifdef __cplusplus
}
#endif