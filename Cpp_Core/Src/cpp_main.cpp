#include "cpp_main.hpp"
#include <stdio.h>
#include <stdint.h>
#include "bsp/board_api.h"
#include "enums.hpp"
#include "drivermanager.hpp"
#include "drivers/net/NetDriver.hpp"
#include "configmanager.hpp"
#include "board_cfg.h"
#include "qspi-w25q64.h"
#include "led.h"
#include "adc.h"
#include "pwm-ws2812b.h"
// #include "net_init.h"


// static __attribute__((section("._ADC_DMA_Area"))) uint32_t ADC_Values[16] = {0};


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // printf("ADC Conv;\n");
}
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc){
    printf("Sorry, ADC error!\n");
}


int cpp_main(void) 
{   

    board_init();
    printf("board_init success ... \r\n");
    
    // tusb_init();
    // tud_init(BOARD_TUD_RHPORT);
    // net_init();

    // InputMode inputMode = INPUT_MODE_CONFIG;
    InputMode inputMode = INPUT_MODE_XINPUT;
    ConfigType configType = CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    bool configMode = false;
    
    // HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    // if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Values[0], sizeof(ADC_Values) / sizeof(ADC_Values[0]) / 2) != HAL_OK) {
    //     printf("DMA1 start fail. \n");
    // } else {
    //     printf("DMA1 start success.\n");
    // }

    // HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
    // if(HAL_ADC_Start_DMA(&hadc2, (uint32_t*)&ADC_Values[8], sizeof(ADC_Values) / sizeof(ADC_Values[0]) / 2) != HAL_OK) {
    //     printf("DMA2 start fail. \n");
    // } else {
    //     printf("DMA2 start success.\n");
    // }

    WS2812B_Init();
    WS2812B_Start();
    WS2812B_SetAllLEDBrightness(5);
    WS2812B_SetAllLEDColor(0, 255, 0);
    uint32_t t = HAL_GetTick();
    uint8_t a = 0;


    while(1) {
        // LED1_Toggle;
        // printf("FS_ROOT\r\n");
        
        // HAL_Delay(1000);

        if(HAL_GetTick() - t >= 1000)
        {

            
            LED1_Toggle;


            
            
            WS2812B_SetLEDBrightness(5, a);
            if(a < 15) {
                a ++;
            } else {
                a = 0;
            }
            WS2812B_SetLEDBrightness(0, a);

            // switch(a) {
            //     case 0:
            //         WS2812B_SetAllLEDColor(255, 0, 0);
            //         break;
            //     case 1:
            //         WS2812B_SetAllLEDColor(0, 255, 0);
            //         break;
            //     default:
            //         WS2812B_SetAllLEDColor(0, 0, 255);
            //         break;
            // }
            
            // WS2812B_SetAllLEDColor((uint8_t) (random() * 255), (uint8_t) (random() * 255), (uint8_t) (random() * 255));
            // 因为开启了DCache，所以每次DMA每次更新完读取是需要使DCache失效
            // SCB_InvalidateDCache_by_Addr((uint32_t *)ADC_Values, sizeof(ADC_Values));

            // printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
            //     ADC_Values[0], ADC_Values[1], ADC_Values[2], ADC_Values[3], ADC_Values[4], ADC_Values[5], ADC_Values[6], ADC_Values[7],
            //     ADC_Values[8], ADC_Values[9], ADC_Values[10], ADC_Values[11], ADC_Values[12], ADC_Values[13], ADC_Values[14], ADC_Values[15]
            //     );
            // a ++;

            // printf("%d,%d\n", ADC1_Values[0], ADC1_Values[1]);


            t = HAL_GetTick();
        }

        if(configMode) {
            ConfigManager::getInstance().loop();
        } else {
            tud_task();
        }
    }

    return 0;
} 