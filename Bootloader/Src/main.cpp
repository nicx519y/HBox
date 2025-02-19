#include "main.h"
#include "update.hpp"
#include "bootloader_config.h"

extern "C" {
    void SystemClock_Config(void);
}

typedef void (*pFunction)(void);

// 全局对象
extern FirmwareUpdater firmwareUpdater;

void jumpToApplication(void)
{
    uint32_t JumpAddress = *(__IO uint32_t*)(APP_ADDRESS + 4);
    pFunction Jump = (pFunction)JumpAddress;
    
    // 关闭所有中断
    __disable_irq();
    
    // 重置所有外设
    HAL_DeInit();
    
    // 设置主堆栈指针
    __set_MSP(*(__IO uint32_t*)APP_ADDRESS);
    
    // 跳转到应用程序
    Jump();
}

int main(void)
{
    // 基础初始化
    HAL_Init();
    SystemClock_Config();
    
    // 检查是否需要更新固件
    if(firmwareUpdater.checkForUpdate()) {
        // 执行固件更新
        firmwareUpdater.performUpdate();
    }
    
    // 检查应用程序有效性
    if(firmwareUpdater.verifyApplication()) {
        // 跳转到应用程序
        jumpToApplication();
    }
    
    // 如果到这里说明出错了,进入错误处理
    Error_Handler();
} 