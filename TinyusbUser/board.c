#include "stm32h7xx_hal.h"
#include "board_cfg.h"
#include "bsp/board_api.h"

UART_HandleTypeDef UartHandle;

void SystemClock_Config(void);
void PeriphCommonClock_Config(void);

void board_init(void) {
  // Implemented in board.h
  SystemClock_Config();

  // Enable All GPIOs clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE(); // USB ULPI NXT
  __HAL_RCC_GPIOC_CLK_ENABLE(); // USB ULPI NXT
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE(); // USB ULPI NXT
#ifdef __HAL_RCC_GPIOI_CLK_ENABLE
  __HAL_RCC_GPIOI_CLK_ENABLE(); // USB ULPI NXT
#endif
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  // trace_etm_init();

  // Enable UART Clock
  UART_CLK_EN();

#if CFG_TUSB_OS == OPT_OS_NONE
  // 1ms tick timer
  SysTick_Config(SystemCoreClock / 1000);

#elif CFG_TUSB_OS == OPT_OS_FREERTOS
  // Explicitly disable systick to prevent its ISR runs before scheduler start
  SysTick->CTRL &= ~1U;

  // If freeRTOS is used, IRQ priority is limit by max syscall ( smaller is higher )
#ifdef USB_OTG_FS_PERIPH_BASE
  NVIC_SetPriority(OTG_FS_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
#endif
  NVIC_SetPriority(OTG_HS_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
#endif

  GPIO_InitTypeDef GPIO_InitStruct;

  // LED
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  // Button
  // GPIO_InitStruct.Pin = BUTTON_PIN;
  // GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  // GPIO_InitStruct.Pull = GPIO_NOPULL;
  // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  // HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);

  // Uart
  GPIO_InitStruct.Pin = UART_TX_PIN | UART_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = UART_GPIO_AF;
  HAL_GPIO_Init(UART_GPIO_PORT, &GPIO_InitStruct);

  UartHandle.Instance = UART_DEV;
  UartHandle.Init.BaudRate = CFG_BOARD_UART_BAUDRATE;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&UartHandle);

#if BOARD_TUD_RHPORT == 0
  // Despite being call USB2_OTG
  // OTG_FS is marked as RHPort0 by TinyUSB to be consistent across stm32 port
  // PA9 VUSB, PA10 ID, PA11 DM, PA12 DP

  // Configure DM DP Pins
  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_HS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // This for ID line debug
  // GPIO_InitStruct.Pin = GPIO_PIN_10;
  // GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  // GPIO_InitStruct.Pull = GPIO_PULLUP;
  // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  // GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_HS;
  // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // https://community.st.com/s/question/0D50X00009XkYZLSA3/stm32h7-nucleo-usb-fs-cdc
  // TODO: Board init actually works fine without this line.
  HAL_PWREx_EnableUSBVoltageDetector();
  __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();

#if OTG_FS_VBUS_SENSE
  // Configure VBUS Pin
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Enable VBUS sense (B device) via pin PA9
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
#else
  // Disable VBUS sense (B device) via pin PA9
  USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

  // B-peripheral session valid override enable
  USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
  USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
#endif // vbus sense

#elif BOARD_TUD_RHPORT == 1
  // Despite being call USB2_OTG
  // OTG_HS is marked as RHPort1 by TinyUSB to be consistent across stm32 port

  struct {
    GPIO_TypeDef* port;
    uint32_t pin;
  } const ulpi_pins[] =
  {
    ULPI_PINS
  };

  for (uint8_t i=0; i < sizeof(ulpi_pins)/sizeof(ulpi_pins[0]); i++)
  {
    GPIO_InitStruct.Pin       = ulpi_pins[i].pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_HS;
    HAL_GPIO_Init(ulpi_pins[i].port, &GPIO_InitStruct);
  }

  // Enable USB HS & ULPI Clocks
  __HAL_RCC_USB1_OTG_HS_ULPI_CLK_ENABLE();
  __HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

#if OTG_HS_VBUS_SENSE
  #error OTG HS VBUS Sense enabled is not implemented
#else
  // No VBUS sense
  USB_OTG_HS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

  // B-peripheral session valid override enable
  USB_OTG_HS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
  USB_OTG_HS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
#endif

  // Force device mode
  USB_OTG_HS->GUSBCFG &= ~USB_OTG_GUSBCFG_FHMOD;
  USB_OTG_HS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;

  HAL_PWREx_EnableUSBVoltageDetector();

  // For waveshare openh743 ULPI PHY reset walkaround
  board_stm32h7_post_init();
#endif // rhport = 1

}

void board_led_write(bool state) {
  HAL_GPIO_WritePin(LED_PORT, LED_PIN, state ? LED_STATE_ON : (1-LED_STATE_ON));
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphCommonClock_Config();
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
   */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL3.PLL3M = 2;
  PeriphClkInitStruct.PLL3.PLL3N = 14;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 12;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOMEDIUM;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /**
   * USB Clock
   * 
   */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

}