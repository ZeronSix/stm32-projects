#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_exti.h"
#include "buttons.h"
#include "buzzer.h"
#include "clock.h"

static void SystemClock_Config(void);

int main(void) {
    SystemClock_Config();
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    Clock_Init();
    Buttons_Init();
    Buzzer_Init();

    while (1) {
        Clock_Update();
    }
    return 0;
}

/**
  * System Clock Configuration
  * The system Clock is configured as follow :
  *    System Clock source            = PLL (HSI/2)
  *    SYSCLK(Hz)                     = 48000000
  *    HCLK(Hz)                       = 48000000
  *    AHB Prescaler                  = 1
  *    APB1 Prescaler                 = 1
  *    HSI Frequency(Hz)              = 8000000
  *    PLLMUL                         = 12
  *    Flash Latency(WS)              = 1
  */
void SystemClock_Config() {
    /* Set FLASH latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

    /* Enable HSI and wait for activation*/
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1);

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2,
                                LL_RCC_PLL_MUL_12);

    LL_RCC_PLL_Enable();
    while (LL_RCC_PLL_IsReady() != 1);

    /* Sysclk activation on the main PLL */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    /* Set APB1 prescaler */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    /* Set systick to 1ms */
    SysTick_Config(48000000/1000);

    /* Update CMSIS variable (which can be updated also
     * through SystemCoreClockUpdate function) */
    SystemCoreClock = 48000000;
}

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);
    while (1);
}

void SVC_Handler(void) {
}

void PendSV_Handler(void) {
}


void SysTick_Handler(void) {
    Clock_UpdateTimer();
    Buzzer_UpdateTimer();
    Buttons_UpdateTimers();
}
