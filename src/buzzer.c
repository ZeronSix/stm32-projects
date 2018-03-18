#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_bus.h"
#include "buzzer.h"

static int signalTimer = 0;
static int buzzTimer = 0;

void Buzzer_Init(void) {
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
}

void Buzzer_Buzz(int time) {
    if (buzzTimer < time) {
        buzzTimer = time;
    }
}

void Buzzer_Stop(void) {
    buzzTimer = 0;
}

void Buzzer_UpdateTimer(void) {
    if (buzzTimer == 0) {
        return;
    }

    signalTimer++;
    if (signalTimer == BUZZ_PERIOD) {
        signalTimer = 0;
        LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_7);
    }
    if (buzzTimer > 0) {
        buzzTimer--;
    }
}

