#include "cpp_main.hpp"
#include <stdio.h>
#include "bsp/board_api.h"
#include "main_state_machine.hpp"
#include "fsdata.h"


int cpp_main(void) 
{   
    board_init();
    printf("board_init success ... \r\n");
    
    // Test FPU Status
    uint32_t fpscr = __get_FPSCR();
    printf("FPSCR = 0x%08lx\r\n", fpscr);
    
    // Detailed FPU Test with volatile variables
    // volatile float test_a = 1.234f;
    // volatile float test_b = 5.678f;
    // volatile float test_c = 2.23f;
    // volatile float test_d = 5.10f;
    // volatile float test_add = test_a + test_b;
    // volatile float test_mul = test_a * test_b;
    // volatile float test_div = test_b / test_a;
    
    // char buf[32];
    // char buf2[32];
    // char buf3[32];
    
    // printf("\r\n=== FPU Test Results ===\r\n");
    // printf("test_a == test_b ? %s\r\n", test_a == test_b ? "YES" : "NO");
    // printf("test_c * test_d == 11.373f ? %s\r\n", test_c * test_d == 11.373f ? "YES" : "NO");
    
    // // 调试代码示例
    // sprintf(buf, "%.3f", 0.6509f);
    // printf("topDeadzone = %s\n", buf);

    // printf("Memory addresses:\r\n");
    // sprintf(buf, "%.3f", (double)test_a);
    // printf("test_a @ %p = %s (hex: 0x%08lx)\r\n", (void*)&test_a, buf, *(uint32_t*)&test_a);
    // sprintf(buf, "%.3f", (double)test_b);
    // printf("test_b @ %p = %s (hex: 0x%08lx)\r\n", (void*)&test_b, buf, *(uint32_t*)&test_b);
    // sprintf(buf, "%.3f", (double)test_add);
    // printf("test_add @ %p = %s (hex: 0x%08lx)\r\n", (void*)&test_add, buf, *(uint32_t*)&test_add);
    // sprintf(buf, "%.3f", (double)test_mul);
    // printf("test_mul @ %p = %s (hex: 0x%08lx)\r\n", (void*)&test_mul, buf, *(uint32_t*)&test_mul);
    // sprintf(buf, "%.3f", (double)test_div);
    // printf("test_div @ %p = %s (hex: 0x%08lx)\r\n", (void*)&test_div, buf, *(uint32_t*)&test_div);
    
    // printf("\r\nOperations:\r\n");
    // sprintf(buf, "%.3f", (double)test_a);
    // sprintf(buf2, "%.3f", (double)test_b);
    // sprintf(buf3, "%.3f", (double)test_add);
    // printf("Addition: %s + %s = %s\r\n", buf, buf2, buf3);
    
    // sprintf(buf3, "%.3f", (double)test_mul);
    // printf("Multiplication: %s * %s = %s\r\n", buf, buf2, buf3);
    
    // sprintf(buf3, "%.3f", (double)test_div);
    // printf("Division: %s / %s = %s\r\n", buf2, buf, buf3);
    // printf("======================\r\n\r\n");
    
    // printf("Value verification:\r\n");
    // volatile float verify_a = test_a;
    // volatile float verify_b = test_b;
    // sprintf(buf, "%.3f", (double)test_a);
    // sprintf(buf2, "%.3f", (double)verify_a);
    // printf("Verify test_a: %s == %s ? %s\r\n", buf, buf2, (test_a == verify_a) ? "YES" : "NO");
    // sprintf(buf, "%.3f", (double)test_b);
    // sprintf(buf2, "%.3f", (double)verify_b);
    // printf("Verify test_b: %s == %s ? %s\r\n", buf, buf2, (test_b == verify_b) ? "YES" : "NO");
    
    Storage::getInstance().init();
    printf("Storage init success.\n");
    
    getFSRoot();
    
    // MainStateMachine::getInstance().setup();
    InputMode inputMode = InputMode::INPUT_MODE_CONFIG;
    // InputMode inputMode = InputMode::INPUT_MODE_XINPUT;
    ConfigType configType = ConfigType::CONFIG_TYPE_WEB;
    DriverManager::getInstance().setup(inputMode);      
    ConfigManager::getInstance().setup(configType);

    Gamepad& gamepad = Gamepad::getInstance();
    gamepad.setup();

    GPDriver * inputDriver = DriverManager::getInstance().getDriver();
    
    bool configMode = true;
    
    // Start the TinyUSB Device functionality
    tud_init(TUD_OPT_RHPORT);

    uint32_t t = HAL_GetTick();

    while(1) {
        if(HAL_GetTick() - t >= 1)
        {
            // LED1_Toggle;
            // gamepad.loop();
            t = HAL_GetTick();
        }

        if(configMode) {
            ConfigManager::getInstance().loop();
        } else {
            inputDriver->process(&gamepad);
            tud_task();
        }
    }

    return 0;
} 