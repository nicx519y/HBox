#include "stm32h7xx.h"

// 系统时钟变量定义
uint32_t SystemCoreClock = 64000000;
uint32_t SystemD2Clock = 64000000;
const uint8_t D1CorePrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

void SystemInit(void)
{
    // 基本系统初始化
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  // Enable FPU
    
    // Reset the RCC clock configuration to the default reset state
    RCC->CR |= RCC_CR_HSION;
    
    // Reset CFGR register
    RCC->CFGR = 0x00000000;
    
    // Reset HSEON, CSSON , CSION,RC48ON, CSIKERON PLL1ON, PLL2ON and PLL3ON bits
    RCC->CR &= 0xEAF6ED7FU;

    // Reset PLLCFGR register
    RCC->PLLCKSELR = 0x02020200;
    RCC->PLLCFGR = 0x01FF0000;
    RCC->PLL1DIVR = 0x01010280;
    RCC->PLL1FRACR = 0x00000000;
    RCC->PLL2DIVR = 0x01010280;
    RCC->PLL2FRACR = 0x00000000;
    RCC->PLL3DIVR = 0x01010280;
    RCC->PLL3FRACR = 0x00000000;
    
    // Reset D1CFGR register
    RCC->D1CFGR = 0x00000000;
    
    // Reset D2CFGR register
    RCC->D2CFGR = 0x00000000;
    
    // Reset D3CFGR register
    RCC->D3CFGR = 0x00000000;
    
    // Reset PLLCKSELR register
    RCC->PLLCKSELR = 0x00000000;
    
    // Reset PLL1DIVR register
    RCC->PLL1DIVR = 0x00000000;
    
    // Reset PLL1FRACR register
    RCC->PLL1FRACR = 0x00000000;
    
    // Reset PLL2DIVR register
    RCC->PLL2DIVR = 0x00000000;
    
    // Reset PLL2FRACR register
    RCC->PLL2FRACR = 0x00000000;
    
    // Reset PLL3DIVR register
    RCC->PLL3DIVR = 0x00000000;
    
    // Reset PLL3FRACR register
    RCC->PLL3FRACR = 0x00000000;
    
    // Reset HSEBYP bit
    RCC->CR &= 0xFFFBFFFF;
    
    // Disable all interrupts
    RCC->CIER = 0x00000000;
    
    // Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7)
    if((DBGMCU->IDCODE & 0xFFFF0000U) < 0x20000000U)
    {
        *((__IO uint32_t*)0x51008108) = 0x00000001U;
    }
}

void SystemCoreClockUpdate(void)
{
    uint32_t pllp, pllsource, pllm, pllfracen, hsivalue, tmp;
    uint32_t common_system_clock;
    float_t fracn1, pllvco;

    // Get SYSCLK source
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
    case RCC_CFGR_SWS_HSI:  // HSI used as system clock source
        SystemCoreClock = HSI_VALUE;
        break;

    case RCC_CFGR_SWS_CSI:  // CSI used as system clock  source
        SystemCoreClock = CSI_VALUE;
        break;

    case RCC_CFGR_SWS_HSE:  // HSE used as system clock  source
        SystemCoreClock = HSE_VALUE;
        break;

    case RCC_CFGR_SWS_PLL1:  // PLL1 used as system clock  source
        // PLL_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLLM) * PLLN
        // SYSCLK = PLL_VCO / PLLR

        pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
        pllm = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1)>> 4);
        pllfracen = ((RCC->PLLCFGR & RCC_PLLCFGR_PLL1FRACEN)>>RCC_PLLCFGR_PLL1FRACEN_Pos);
        fracn1 = (float_t)(uint32_t)(pllfracen* ((RCC->PLL1FRACR & RCC_PLL1FRACR_FRACN1)>> 3));

        if (pllm != 0U)
        {
            switch (pllsource)
            {
            case RCC_PLLCKSELR_PLLSRC_HSI:  // HSI used as PLL clock source
                hsivalue = (HSI_VALUE >> ((RCC->CR & RCC_CR_HSIDIV)>> 3)) ;
                pllvco = ( (float_t)hsivalue / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
                break;

            case RCC_PLLCKSELR_PLLSRC_CSI:  // CSI used as PLL clock source
                pllvco = ((float_t)CSI_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
                break;

            case RCC_PLLCKSELR_PLLSRC_HSE:  // HSE used as PLL clock source
                pllvco = ((float_t)HSE_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
                break;

            default:
                pllvco = ((float_t)CSI_VALUE / (float_t)pllm) * ((float_t)(uint32_t)(RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/(float_t)0x2000) +(float_t)1 );
                break;
            }
            pllp = (((RCC->PLL1DIVR & RCC_PLL1DIVR_P1) >>9) + 1U ) ;
            SystemCoreClock =  (uint32_t)(float_t)(pllvco/(float_t)pllp);
        }
        else
        {
            SystemCoreClock = 0U;
        }
        break;

    default:
        SystemCoreClock = CSI_VALUE;
        break;
    }

    // Compute SystemClock frequency
    tmp = D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE)>> RCC_D1CFGR_D1CPRE_Pos];
    common_system_clock = (SystemCoreClock >> tmp);

    SystemD2Clock = (common_system_clock >> ((D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_HPRE)>> RCC_D1CFGR_HPRE_Pos]) & 0x1FU));
} 