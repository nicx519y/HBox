#include "main.h"
#include "update.hpp"
#include "bootloader_config.h"
#include "w25qxx.h"

extern "C" {
    void SystemClock_Config(void);
}

typedef void (*pFunction)(void);

// 全局对象
extern FirmwareUpdater firmwareUpdater;

void jumpToApplication(void)
{
    // 初始化QSPI为内存映射模式
    if(W25QXX_EnableMemoryMappedMode() != W25QXX_OK) {
        Error_Handler();
    }
    
    // 获取应用程序入口点
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
    // 初始化 HAL 库
    HAL_Init();
    
    // 配置系统时钟
    SystemClock_Config();
    
    // 初始化QSPI Flash
    if(W25QXX_Init() != W25QXX_OK) {
        Error_Handler();
    }
    
    // 检查是否需要更新
    if(firmwareUpdater.checkForUpdate()) {
        if(firmwareUpdater.performUpdate()) {
            // 更新成功，重启
            NVIC_SystemReset();
        }
    }
    
    // 验证应用程序
    if(firmwareUpdater.verifyApplication()) {
        // 跳转到应用程序
        jumpToApplication();
    }
    
    // 如果到这里说明出错了
    Error_Handler();
} 