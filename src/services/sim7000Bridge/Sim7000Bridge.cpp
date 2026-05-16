#include "Sim7000Bridge.hpp"
#include "app/App.hpp"

extern App app;

extern "C" void sim7000_uart_rx_event(uint16_t len)
{
    app.getSim7000().onRxEvent(len);
}

extern "C" uint8_t* sim7000_get_rx_buffer(void)
{
    return app.getSim7000().getRxBuffer();
}

extern "C" uint16_t sim7000_get_rx_buffer_size(void)
{
    return app.getSim7000().getRxBufferSize();
}