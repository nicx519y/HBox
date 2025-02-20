#ifndef __LED_H
#define __LED_H

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------ LED配置宏 ----------------------------------*/

#define LED1_PIN                GPIO_PIN_3
#define LED1_PORT              GPIOE
#define __HAL_RCC_LED1_CLK_ENABLE()   __HAL_RCC_GPIOE_CLK_ENABLE()
 

  
/*----------------------------------------- LED控制宏 ----------------------------------*/
						
#define LED1_ON 	  	                HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET)  // 输出低电平，点亮LED1	
#define LED1_OFF 	  	                HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET)    // 输出高电平，关闭LED1	
#define LED1_Toggle	                    HAL_GPIO_TogglePin(LED1_PORT,LED1_PIN);					// 翻转IO口状态
			
/*---------------------------------------- 函数声明 ------------------------------------*/

void LED_Init(void);

#ifdef __cplusplus
}
#endif

#endif //__LED_H


