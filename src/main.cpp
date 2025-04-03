#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f407xx.h"
#include "stm32f4xx.h"

#include "hydrv_clock.h"
#include "hydrv_common.h"
#include "hydrv_gpio.h"

#ifdef __cplusplus
}
#endif

#include "hydrv_serial_protocol.hpp"
#include "hydrv_uart.hpp"

#define BUFFER_LENGTH 10

class Memory : public hydrolib::serialProtocol::MessageProcessor::PublicMemoryInterface
{
public:
  Memory(uint8_t *buffer,
         uint32_t length) : buffer_(buffer),
                            length_(length)
  {
  }

public:
  hydrolib_ReturnCode Read(void *buffer, uint32_t address,
                           uint32_t length) override
  {
    memcpy(buffer, buffer_ + address, length);
    return HYDROLIB_RETURN_OK;
  }

  hydrolib_ReturnCode Write(const void *buffer, uint32_t address,
                            uint32_t length) override
  {
    memcpy(buffer_ + address, buffer, length);
    return HYDROLIB_RETURN_OK;
  }

  uint32_t Size() override
  {
    return length_;
  }

private:
  uint8_t *buffer_;
  uint32_t length_;
};

uint8_t buffer[BUFFER_LENGTH];

Memory memory(buffer, BUFFER_LENGTH);
hydrv::serialProtocol::SerialProtocolDriver protocol_handler(2, memory, hydrv::UART::UARTLow::USART3_LOW,
                                                             GPIOC, HYDRV_GPIO_PIN_11, GPIOC, HYDRV_GPIO_PIN_10, 7);

// hydrv::UART::UART<100, 100> uart(hydrv::UART::UART<100, 100>::HYDRV_USART3,
//                                  GPIOC, HYDRV_GPIO_PIN_11, GPIOC, HYDRV_GPIO_PIN_10, 7);

extern "C"
{
  void UART_IT_Handler()
  {

    protocol_handler.IRQHandler();
  }
}

int main(void)
{
  hydrv_Clock_ConfigureHSI();
  HYDRV_ENABLE_GPIO_CLOCK(GPIOD);
  NVIC_SetPriorityGrouping(0);
  hydrv_GPIO_InitOutput(GPIOD, HYDRV_GPIO_PIN_15, 0);

  while (1)
  {

    protocol_handler.ProcessRx();
    if (buffer[0] == 'a')
    {
      hydrv_GPIO_Set(GPIOD, HYDRV_GPIO_PIN_15);
    }
    else if (buffer[0] == 'b')
    {
      hydrv_GPIO_Reset(GPIOD, HYDRV_GPIO_PIN_15);
    }

    // uart.Transmit(buffer, 6);

    // hydrv_Clock_Delay(500);

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
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
