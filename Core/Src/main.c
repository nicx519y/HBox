/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "memorymap.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "usbd_desc.h"
// #include "usbd_core.h"
// // #include "usbd_cdc.h"
// #include "usbd_composite_builder.h"
// #include "usbd_cdc_rndis.h"
// #include "usbd_cdc_rndis_if.h"

#include "bsp/board_api.h"
#include "tusb.h"
#include "dhserver.h"
#include "dnserver.h"
#include "httpd.h"
#include "lwip/ethip6.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// USBD_HandleTypeDef USBD_Device;

#ifdef INCLUDE_IPERF
#include "lwip/apps/lwiperf.h"
#endif

#define INIT_IP4(a, b, c, d) \
  {PP_HTONL(LWIP_MAKEU32(a, b, c, d))}

/* lwip context */
static struct netif netif_data;

/* shared between tud_network_recv_cb() and service_traffic() */
static struct pbuf *received_frame;

/* this is used by this code, ./class/net/net_driver.c, and usb_descriptors.c */
/* ideally speaking, this should be generated from the hardware's unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
uint8_t tud_network_mac_address[6] = {0x02, 0x02, 0x84, 0x6A, 0x96, 0x00};

/* network parameters of this MCU */
static const ip4_addr_t ipaddr = INIT_IP4(192, 168, 7, 1);
static const ip4_addr_t netmask = INIT_IP4(255, 255, 255, 0);
static const ip4_addr_t gateway = INIT_IP4(0, 0, 0, 0);

/* database IP addresses that can be offered to the host; this must be in RAM to store assigned MAC addresses */
static dhcp_entry_t entries[] = {
    /* mac ip address               lease time */
    {{0}, INIT_IP4(192, 168, 7, 2), 24 * 60 * 60},
    {{0}, INIT_IP4(192, 168, 7, 3), 24 * 60 * 60},
    {{0}, INIT_IP4(192, 168, 7, 4), 24 * 60 * 60},
};

static const dhcp_config_t dhcp_config = {
    .router = INIT_IP4(0, 0, 0, 0),  /* router address (if any) */
    .port = 67,                      /* listen port */
    .dns = INIT_IP4(192, 168, 7, 1), /* dns server (if any) */
    "usb",                           /* dns suffix */
    TU_ARRAY_SIZE(entries),          /* num entry */
    entries                          /* entries */
};

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
  (void)netif;

  for (;;)
  {
    /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
    if (!tud_ready())
      return ERR_USE;

    /* if the network driver can accept another packet, we make it happen */
    if (tud_network_can_xmit(p->tot_len))
    {
      tud_network_xmit(p, 0 /* unused for this example */);
      return ERR_OK;
    }

    /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
    tud_task();
  }
}

static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr)
{
  return etharp_output(netif, p, addr);
}

#if LWIP_IPV6
static err_t ip6_output_fn(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr)
{
  return ethip6_output(netif, p, addr);
}
#endif

static err_t netif_init_cb(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = ip4_output_fn;
#if LWIP_IPV6
  netif->output_ip6 = ip6_output_fn;
#endif
  return ERR_OK;
}

static void init_lwip(void)
{
  struct netif *netif = &netif_data;

  lwip_init();

  /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
  netif->hwaddr_len = sizeof(tud_network_mac_address);
  memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
  netif->hwaddr[5] ^= 0x01;

  netif = netif_add(netif, &ipaddr, &netmask, &gateway, NULL, netif_init_cb, ip_input);
#if LWIP_IPV6
  netif_create_ip6_linklocal_address(netif, 1);
#endif
  netif_set_default(netif);
}

/* handle any DNS requests from dns-server */
bool dns_query_proc(const char *name, ip4_addr_t *addr)
{
  if (0 == strcmp(name, "tiny.usb"))
  {
    *addr = ipaddr;
    return true;
  }
  return false;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
  /* this shouldn't happen, but if we get another packet before
  parsing the previous, we must signal our inability to accept it */
  if (received_frame)
    return false;

  if (size)
  {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p)
    {
      /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
      memcpy(p->payload, src, size);

      /* store away the pointer for service_traffic() to later handle */
      received_frame = p;
    }
  }

  return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
  struct pbuf *p = (struct pbuf *)ref;

  (void)arg; /* unused for this example */

  return pbuf_copy_partial(p, dst, p->tot_len, 0);
}

static void service_traffic(void)
{
  /* handle any packet received by tud_network_recv_cb() */
  if (received_frame)
  {
    ethernet_input(received_frame, &netif_data);
    pbuf_free(received_frame);
    received_frame = NULL;
    tud_network_recv_renew();
  }

  sys_check_timeouts();
}

void tud_network_init_cb(void)
{
  /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
  if (received_frame)
  {
    pbuf_free(received_frame);
    received_frame = NULL;
  }
}

/* lwip has provision for using a mutex, when applicable */
sys_prot_t sys_arch_protect(void)
{
  return 0;
}
void sys_arch_unprotect(sys_prot_t pval)
{
  (void)pval;
}

/* lwip needs a millisecond time source, and the TinyUSB board support code has one available */
uint32_t sys_now(void)
{
  // return board_millis();
  return HAL_GetTick();
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  // SystemClock_Config();

  /* Configure the peripherals common clocks */
  // PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  // MX_GPIO_Init();
  // MX_DMA_Init();
  // MX_ADC1_Init();
  // MX_ADC2_Init();
  // MX_USART1_UART_Init();
  // MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  // USBD_Init(&USBD_Device, &FS_Desc, DEVICE_FS);
  // USBD_CMPSIT_AddClass(&USBD_Device, USBD_CDC_RNDIS_CLASS);
  // while (USBD_CMPSIT_AddToConfDesc(&USBD_Device) != USBD_OK)
  //   ;
  // while (USBD_RegisterClass(&USBD_Device, USBD_CDC_RNDIS_CLASS) != USBD_OK)
  //   ;
  // while (USBD_CDC_RNDIS_RegisterInterface(&USBD_Device, USBD_CDC_RNDIS_ITF) != USBD_OK)
  //   ;
  // while (USBD_Start(&USBD_Device) != USBD_OK)
  //   ;
  // HAL_PWREx_EnableUSBVoltageDetector();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  board_init();

  board_led_write(false);

  // tusb_init();
  tud_init(BOARD_TUD_RHPORT);
  init_lwip();
  while (!netif_is_up(&netif_data));
  while (dhserv_init(&dhcp_config) != ERR_OK);
  while (dnserv_init(IP_ADDR_ANY, 53, dns_query_proc) != ERR_OK);
  httpd_init();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    tud_task();
    service_traffic();
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
   */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
