#include "sys.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32F429������
//ϵͳʱ�ӳ�ʼ��
//����ʱ������/�жϹ���/GPIO���õ�
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/5
//�汾��V1.0
//��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//********************************************************************************
//�޸�˵��
//��
//////////////////////////////////////////////////////////////////////////////////

//ʱ��ϵͳ���ú���
// Fvco=Fs*(plln/pllm);
// SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
// Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

// Fvco:VCOƵ��
// SYSCLK:ϵͳʱ��Ƶ��
// Fusb:USB,SDIO,RNG�ȵ�ʱ��Ƶ��
// Fs:PLL����ʱ��Ƶ��,������HSI,HSE��.
// plln:��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
// pllm:��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
// pllp:ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
// pllq:USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=360,pllm=25,pllp=2,pllq=8.
//�õ�:Fvco=25*(360/25)=360Mhz
//     SYSCLK=360/2=180Mhz
//     Fusb=360/8=45Mhz
//����ֵ:0,�ɹ�;1,ʧ��
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{

    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
//��������ʾ�����ʱ��˺����������������ļ���������
// file��ָ��Դ�ļ�
// line��ָ�����ļ��е�����
void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
    {
    }
}
#endif

// THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI
__asm void WFI_SET(void)
{
    WFI;
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
    CPSID I
        BX LR
}
//���������ж�
__asm void INTX_ENABLE(void)
{
    CPSIE I
        BX LR
}
//����ջ����ַ
// addr:ջ����ַ
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 // set Main Stack value
                 BX r14
}
