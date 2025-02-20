#include "cpp_main.hpp"
#include <stdio.h>
#include "bsp/board_api.h"
#include "board_cfg.h"
#include "main_state_machine.hpp"
#include "fsdata.h"
#include "led.h"
#include "qspi-w25q64.h"
#include "message_center.hpp"
#include "adc.h"

extern "C" {
    int cpp_main(void) 
    {   
        board_init();
        APP_DBG("cpp_main: board_init success.");

        LED1_ON;
        APP_DBG("cpp_main: LED1_ON success.");
        
        // Test FPU Status
        uint32_t fpscr = __get_FPSCR();
        APP_DBG("cpp_main: FPSCR = 0x%08lx", fpscr);

        // 注册ADC消息
        MC.registerMessage(MessageId::DMA_ADC_CONV_CPLT);
        MC.registerMessage(MessageId::ADC_BTNS_STATE_CHANGED);

        getFSRoot();
        APP_DBG("cpp_main: getFSRoot success.");

        MAIN_STATE_MACHINE.setup();

        return 0;
    }
}

// ADC转换完成回调
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    MC.publish(MessageId::DMA_ADC_CONV_CPLT, hadc);
}

// ADC错误回调
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    uint32_t error = HAL_ADC_GetError(hadc);
    printf("ADC Error: Instance=0x%p\n", (void*)hadc->Instance);
    printf("State=0x%x\n", HAL_ADC_GetState(hadc));
    printf("Error flags: 0x%lx\n", error);
    
    if (error & HAL_ADC_ERROR_INTERNAL) printf("- Internal error\n");
    if (error & HAL_ADC_ERROR_OVR) printf("- Overrun error\n");
    if (error & HAL_ADC_ERROR_DMA) printf("- DMA transfer error\n");
}

