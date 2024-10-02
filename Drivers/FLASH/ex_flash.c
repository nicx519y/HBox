#include "ex_flash.h"
#include "stm32_hal_legacy.h"
#include "stm32h7xx_hal_flash_ex.h"
#include <stdio.h>

/**
  * @brief 往内部FLASH写入数据
  * @param addr 写入地址
  * @param pdata 存储待写数据
  * @retval None
  */
void FLASH_Inside_Wr(uint32_t addr,uint32_t Pdata)
{
    //定义局部变量
    uint32_t PageError = 0;
    HAL_StatusTypeDef HAL_Status;
    //擦除配置信息结构体，包括擦除地址、方式、页数等
    FLASH_EraseInitTypeDef pEraseInit;
    pEraseInit.TypeErase = TYPEERASE_SECTORS;//按页擦除
    pEraseInit.Sector = addr;//擦除地址
    pEraseInit.NbSectors = 1;//擦除页数量
    //step1 解锁内部FLASH，允许读写功能
    HAL_FLASH_Unlock();
    //step2 开始擦除addr对应页
    HAL_Status = HAL_FLASHEx_Erase(&pEraseInit,&PageError);//擦除
    if(HAL_Status != HAL_OK) printf("内部FlASH擦除失败！rn");
    //step3 写入数据
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,addr,Pdata);
    //step4 锁定FLASH
    HAL_FLASH_Lock();
}


/**
  * @brief 读取内部FLASH数据
  * @param addr 读取地址
  * @retval 读出的数据
  */
uint32_t FLASH_Inside_Rd(uint32_t addr)
{
    //定义局部变量
    uint32_t RdData = 0;
    //step1 解锁内部FLASH，允许读写功能
    HAL_FLASH_Unlock();
    RdData = *(__IO uint32_t *)addr;
    //step2 锁定FLASH
    HAL_FLASH_Lock();

    return RdData;
}

/**
  * @brief 内部FLASH读写测试
  */
void FLASH_Inside_Test(void)
{
    printf("\r\n------------------内部FLASH读写测试------------------\r\n");
    uint32_t addr = 0x08008A70;//确保该地址内部FLASH是空余的！
    uint32_t WrData = 0x01234567;//待写入数据
    uint32_t RdData = 0;//存储读取数据

    printf("地址0x%x写入数据：0x%x\r\n",addr,WrData);
    FLASH_Inside_Wr(addr,WrData);//写入数据
    RdData = FLASH_Inside_Rd(addr);//读取数据
    printf("地址0x%x读出数据：0x%x\r\n",addr,RdData);
}