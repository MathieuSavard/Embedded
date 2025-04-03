#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/uart.h"

#include "esp_log.h"

#define UART UART_NUM_0
#define UART_TX 13
#define UART_RX 12

#define VALIDATE(status)                                                       \
    {                                                                          \
        if (ESP_OK != status) {                                                \
            ESP_LOGE("ERROR",                                                  \
                     "function failed with error code: \"%d\" at line \"%d\"", \
                     status, __LINE__);                                        \
            return;                                                            \
        }                                                                      \
    }

void app_main(void) {
    uart_config_t uart_config = {.baud_rate = 9600,
                                 .data_bits = UART_DATA_8_BITS,
                                 .parity = UART_PARITY_DISABLE,
                                 .stop_bits = UART_STOP_BITS_1,
                                 .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                                 .rx_flow_ctrl_thresh = 0,
                                 .source_clk = UART_SCLK_DEFAULT};

    if (uart_is_driver_installed(UART))
        VALIDATE(uart_driver_delete(UART))
    VALIDATE(uart_driver_install(UART, 0, 0, 8, NULL, 0)) // Todo: check last parameter
    VALIDATE(uart_param_config(UART, &uart_config))
    VALIDATE(uart_set_pin(UART, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE))

    // VALIDATE(uart_enable_rx_intr(UART))

    const char character = 'x';
    while (1) {
        VALIDATE(uart_tx_chars(UART, &character, sizeof(character)))
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}