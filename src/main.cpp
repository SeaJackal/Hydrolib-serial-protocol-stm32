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

#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

#include "hydros_serial_protocol.hpp"
#include "hydros_logger.hpp"

#define BUFFER_LENGTH 10

uint8_t buffer[10];

// hydros::serialProtocol::SerialProtocolModule serial_protocol(
//     "SerialProtocol",
//     osPriorityNormal, USART3, 2, buffer, 10);

hydros::logger::LoggerModule logger_module;

hydrolib::Logger::Logger logger("Test logger", &logger_module);
hydrolib::Logger::Logger logger2("Test logger2", &logger_module);

hydros::logger::LoggerModule::UARTloggerStream UART_stream(USART3, osPriorityIdle);

extern "C"
{
    void UART_IT_Handler()
    {
        UART_stream.TransmitByte();
        // serial_protocol.UARTinterruptHandler();
    }
}

extern "C"
{
    void StartDefaultTask(void *argument)
    {
        while (1)
        {
            logger.WriteLog(hydrolib::Logger::LogLevel::INFO, "Hello!");
            // if (buffer[0] == 'a')
            // {
            //     hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
            // }
            // else
            // {
            //     hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
            // }
            osDelay(1000);
        }
        // (void)argument;
        // while (1)
        // {
        //     // osSemaphoreAcquire(rx_completed, osWaitForever);
        //     if (buffer[0] == 'a')
        //     {
        //         hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
        //     }
        //     else
        //     {
        //         hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
        //     }
        // }
    }

    void StartDefaultTask2(void *argument)
    {
        while (1)
        {
            logger2.WriteLog(hydrolib::Logger::LogLevel::WARNING, "Hello too!");
            osDelay(500);
        }
        // (void)argument;
        // while (1)
        // {
        //     // osSemaphoreAcquire(rx_completed, osWaitForever);
        //     if (buffer[0] == 'a')
        //     {
        //         hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
        //     }
        //     else
        //     {
        //         hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
        //     }
        // }
    }
}

int main(void)
{
    hydrv_Clock_ConfigureHSI();
    HYDRV_ENABLE_GPIO_CLOCK(GPIOD);
    HYDRV_ENABLE_GPIO_CLOCK(GPIOC);
    hydrv_GPIO_InitOutput(GPIOD, HYDRV_GPIO_PIN_15, false);
    hydrv_GPIO_InitUART_1_3(GPIOC, HYDRV_GPIO_PIN_10);
    hydrv_GPIO_InitUART_1_3(GPIOC, HYDRV_GPIO_PIN_11);
    HYDRV_ENABLE_UART_2_5_AND_7_8_CLOCK(USART3);
    NVIC_SetPriorityGrouping(0);
    NVIC_SetPriority(USART3_IRQn, 7);
    NVIC_EnableIRQ(USART3_IRQn);

    hydrv_UART_Init(USART3);

    osKernelInitialize();

    osThreadAttr_t defaultTask_attributes =
        {
            .name = "DefaultThread",
            .stack_size = 128 * 16,
            .priority = osPriorityBelowNormal,
        };
    osThreadId_t defaultTaskHandle = osThreadNew(StartDefaultTask, nullptr, &defaultTask_attributes);

    osThreadAttr_t defaultTask2_attributes =
        {
            .name = "DefaultThread2",
            .stack_size = 128 * 16,
            .priority = osPriorityNormal,
        };
    osThreadId_t defaultTask2Handle = osThreadNew(StartDefaultTask2, nullptr, &defaultTask2_attributes);

    logger_module.AddSubscriber(UART_stream, hydrolib::Logger::LogLevel::DEBUG, nullptr);

    osKernelStart();

    while (1)
    {

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
