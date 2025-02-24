#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#include "hydrv_common.h"
#include "hydrv_clock.h"
#include "hydrv_gpio.h"
#include "hydrv_uart.h"

#include "hydrolib_serial_protocol_pack.hpp"

#ifdef __cplusplus
}
#endif

#define BUFFER_LENGTH 10

volatile uint8_t buffer[10];

class TxQueue : public hydrolib::serialProtocol::SerialProtocolHandler::TxQueueInterface
{
public:
    hydrolib_ReturnCode Push(void *buffer, uint32_t length) override
    {
        (void)buffer;
        (void)length;
        return HYDROLIB_RETURN_FAIL;
    }
};

int main(void)
{
    hydrv_Clock_ConfigureHSI();
    HYDRV_ENABLE_GPIO_CLOCK(GPIOD);
    HYDRV_ENABLE_GPIO_CLOCK(GPIOC);
    hydrv_GPIO_InitOutput(GPIOD, HYDRV_GPIO_PIN_15, false);
    hydrv_GPIO_InitUART_1_3(GPIOC, HYDRV_GPIO_PIN_10);
    hydrv_GPIO_InitUART_1_3(GPIOC, HYDRV_GPIO_PIN_11);
    HYDRV_ENABLE_UART_2_5_AND_7_8_CLOCK(USART3);
    // NVIC_EnableIRQ(USART3_IRQn);
    hydrv_UART_Init(USART3);

    TxQueue tx_queue;

    hydrolib::serialProtocol::SerialProtocolHandler sp_handler(2, tx_queue, const_cast<uint8_t *>(buffer), 10);

    uint8_t byte;

    while (1)
    {
        if (hydrv_UART_Receive(USART3, &byte) == HYDRV_OK)
        {
            sp_handler.Receive(&byte, 1);
            sp_handler.ProcessRx();
            if (buffer[0] == 'a')
            {
                hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
            }
            else
            {
                hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
            }
        }
        // hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
        // hydrv_Clock_Delay(500);
        // hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
        // hydrv_Clock_Delay(500);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
