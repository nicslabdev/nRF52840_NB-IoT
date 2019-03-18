/***************************************************************************************/
/*
 * nRF52840-AT_terminal
 * Created by Manuel Montenegro, Mar 14, 2019.
 * Developed for MOTAM project.
 *
 *  This application stablish a bypass connection between USB UART and Arduino serial 
 *  pins UART, i.e. D0 and D1 pins.
 *
 *  This is used in order to manage NB-IoT module directly through AT commands.
 *
 *  This code has been developed for Nordic Semi nRF52840 DK and NB-IoT Arduino shield.
*/
/***************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_drv_power.h"
#include "nrf_serial.h"
#include "app_timer.h"
#include "app_error.h"
#include "app_util.h"
#include "boards.h"


// ======== Serial configuration ========

#define OP_QUEUES_SIZE          3
#define APP_TIMER_PRESCALER     NRF_SERIAL_APP_TIMER_PRESCALER

#define SERIAL_FIFO_TX_SIZE 512
#define SERIAL_FIFO_RX_SIZE 512

#define SERIAL_BUFF_TX_SIZE 1
#define SERIAL_BUFF_RX_SIZE 1

// Arduino NB-IoT shield uses Arduino D0 and D1 pins as serial RX and TX
#define ARDUINO_RX_PIN          NRF_GPIO_PIN_MAP(1,1)
#define ARDUINO_TX_PIN          NRF_GPIO_PIN_MAP(1,2)


static void sleep_handler(void)
{
    __WFE();
    __SEV();
    __WFE();
}

NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uarte0_drv_config,
                      RX_PIN_NUMBER, TX_PIN_NUMBER,
                      RTS_PIN_NUMBER, CTS_PIN_NUMBER,
                      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED,
                      NRF_UART_BAUDRATE_115200,
                      UART_DEFAULT_CONFIG_IRQ_PRIORITY);

NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uarte1_drv_config,
                      ARDUINO_RX_PIN, ARDUINO_TX_PIN,
                      RTS_PIN_NUMBER, CTS_PIN_NUMBER,
                      NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED,
                      NRF_UART_BAUDRATE_115200,
                      UART_DEFAULT_CONFIG_IRQ_PRIORITY);


NRF_SERIAL_QUEUES_DEF(serial0_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);
NRF_SERIAL_QUEUES_DEF(serial1_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);


NRF_SERIAL_BUFFERS_DEF(serial0_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);
NRF_SERIAL_BUFFERS_DEF(serial1_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);


NRF_SERIAL_CONFIG_DEF(serial0_config, NRF_SERIAL_MODE_DMA,
                      &serial0_queues, &serial0_buffs, NULL, sleep_handler);
NRF_SERIAL_CONFIG_DEF(serial1_config, NRF_SERIAL_MODE_DMA,
                      &serial1_queues, &serial1_buffs, NULL, sleep_handler);


NRF_SERIAL_UART_DEF(serial0_uarte, 0);
NRF_SERIAL_UART_DEF(serial1_uarte, 1);


int main(void)
{
    ret_code_t ret;

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    ret = nrf_drv_power_init(NULL);
    APP_ERROR_CHECK(ret);

    nrf_drv_clock_lfclk_request(NULL);
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    ret = nrf_serial_init(&serial0_uarte, &m_uarte0_drv_config, &serial0_config);
    APP_ERROR_CHECK(ret);

    ret = nrf_serial_init(&serial1_uarte, &m_uarte1_drv_config, &serial1_config);
    APP_ERROR_CHECK(ret);

    char c;

    while (true)
    {
        
        ret = nrf_serial_read(&serial0_uarte, &c, sizeof(c), NULL, 10);
        while (ret == NRF_SUCCESS)
        {
            nrf_serial_write(&serial1_uarte, &c, sizeof(c), NULL, 0);
            ret = nrf_serial_read(&serial0_uarte, &c, sizeof(c), NULL, 10);
            if (ret != NRF_SUCCESS) {
                nrf_serial_flush(&serial1_uarte, 0);
            }
        }

        ret = nrf_serial_read(&serial1_uarte, &c, sizeof(c), NULL, 10);
        while (ret == NRF_SUCCESS)
        {
            nrf_serial_write(&serial0_uarte, &c, sizeof(c), NULL, 0);
            ret = nrf_serial_read(&serial1_uarte, &c, sizeof(c), NULL, 10);
            if (ret != NRF_SUCCESS) {
                nrf_serial_flush(&serial0_uarte, 0);
            }
        }
    }
}
