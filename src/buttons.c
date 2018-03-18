#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_bus.h"
#include "buttons.h"
#include "clock.h"
#include "buzzer.h"

// TODO: refactor names
enum {
    BTN_STATE_IDLE,
    BTN_STATE_PRESSED,
    BTN_STATE_RELEASED,
    BTN_STATE_ONCE_IDLE,
    BTN_STATE_ONCE_PRESSED
};

typedef struct ButtonState {
    int btn;
    int pin;
    int rawKey;
    int prevRealKey;
    int realKey;
    int rawTicks;
    int realTicks;
    int state;
    void (*onClick)(void);
    void (*onDoubleClick)(void);
} ButtonState;

static ButtonState states[BUTTON_COUNT];

static void ButtonSetTime_OnClick(void) {
    Buzzer_Buzz(BUZZ_BUTTON);
    if (Clock_GetMode() == CLOCKMODE_SETTIME) {
        Clock_ResetClockSeconds();
        Clock_SetMode(CLOCKMODE_CLOCK);
    }
    else {
        Clock_SetMode(CLOCKMODE_SETTIME);
    }
}

static void ButtonSetTime_OnDoubleClick(void) {
    Buzzer_Buzz(BUZZ_BUTTON);
    if (Clock_GetMode() == CLOCKMODE_SETALARM) {
        Clock_ResetAlarmSeconds();
        Clock_SetMode(CLOCKMODE_CLOCK);
    }
    else {
        Clock_SetMode(CLOCKMODE_SETALARM);
    }
}

static void ButtonInc_OnClick(void) {
    Buzzer_Buzz(BUZZ_BUTTON);
    if (Clock_GetMode() == CLOCKMODE_SETTIME) {
        Clock_AddClockTime(60);
    }
    else if (Clock_GetMode() == CLOCKMODE_SETALARM) {
        Clock_AddAlarmTime(60);
    }
}

static void ButtonInc_OnDoubleClick(void) {
    Buzzer_Buzz(BUZZ_BUTTON);
    if (Clock_GetMode() == CLOCKMODE_SETTIME) {
        Clock_AddClockTime(3600);
    }
    else if (Clock_GetMode() == CLOCKMODE_SETALARM) {
        Clock_AddAlarmTime(3600);
    }
}

static void ButtonAlarm_OnClick(void) {
    Buzzer_Stop();
}

static void ButtonAlarm_OnDoubleClick(void) {
    Buzzer_Buzz(BUZZ_BUTTON);
    Clock_ResetAlarm();
    if (Clock_GetMode() == CLOCKMODE_SETALARM) {
        Clock_SetMode(CLOCKMODE_CLOCK);
    }
}

static void UpdateBtnState(int stateIndex) {
    int rawKey = LL_GPIO_IsInputPinSet(GPIOA, states[stateIndex].pin);
    if (rawKey != states[stateIndex].rawKey) {
        states[stateIndex].rawKey = rawKey;
        states[stateIndex].rawTicks = 0;
    }
}

void EXTI0_1_IRQHandler(void) {
    int stateIndex = BUTTON_SETTIME;
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        stateIndex = BUTTON_SETTIME;
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1)) {
        stateIndex = BUTTON_INC;
    }

    UpdateBtnState(stateIndex);

    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
}

void EXTI2_3_IRQHandler(void) {
    UpdateBtnState(BUTTON_ALARM_RESET);

            LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
}

void Buttons_Init(void) {
    states[BUTTON_SETTIME].pin = LL_GPIO_PIN_0;
    states[BUTTON_SETTIME].btn = BUTTON_SETTIME;
    states[BUTTON_SETTIME].onClick = ButtonSetTime_OnClick;
    states[BUTTON_SETTIME].onDoubleClick = ButtonSetTime_OnDoubleClick;
    LL_GPIO_SetPinMode(GPIOA, states[BUTTON_SETTIME].pin,
                       LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, states[BUTTON_SETTIME].pin,
                       LL_GPIO_PULL_NO);

    states[BUTTON_INC].pin = LL_GPIO_PIN_1;
    states[BUTTON_INC].btn = BUTTON_INC;
    states[BUTTON_INC].onClick = ButtonInc_OnClick;
    states[BUTTON_INC].onDoubleClick = ButtonInc_OnDoubleClick;
    LL_GPIO_SetPinMode(GPIOA, states[BUTTON_INC].pin,
                       LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, states[BUTTON_INC].pin,
                       LL_GPIO_PULL_DOWN);

    states[BUTTON_ALARM_RESET].pin = LL_GPIO_PIN_2;
    states[BUTTON_ALARM_RESET].btn = BUTTON_ALARM_RESET;
    states[BUTTON_ALARM_RESET].onClick = ButtonAlarm_OnClick;
    states[BUTTON_ALARM_RESET].onDoubleClick = ButtonAlarm_OnDoubleClick;
    LL_GPIO_SetPinMode(GPIOA, states[BUTTON_ALARM_RESET].pin,
                       LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, states[BUTTON_ALARM_RESET].pin,
                       LL_GPIO_PULL_DOWN);

    // Configure EXTI
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE1);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE2);

    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_0);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_0);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_1);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_2);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_2);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_2);

    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI0_1_IRQn, 0);
    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_SetPriority(EXTI2_3_IRQn, 0);
}

static void UpdateButtonState(ButtonState *btnState) {
    btnState->rawTicks++;
    btnState->realTicks++;

    int oldRawTicks = btnState->rawTicks;
    int oldRealTicks = btnState->realTicks;
    if (oldRawTicks >= BUTTONS_SWITCH_TIMEOUT) {
        btnState->rawTicks = 0;
        if (btnState->rawKey != btnState->realKey) {
            btnState->realTicks = 0;
            btnState->prevRealKey = btnState->realKey;
            btnState->realKey = btnState->rawKey;
        }
    }

    int key = btnState->realKey;
    switch (btnState->state) {
    case BTN_STATE_IDLE:
        if (key) {
            btnState->state = BTN_STATE_PRESSED;
        }
        break;
    case BTN_STATE_PRESSED:
        if (!key) {
            btnState->state = BTN_STATE_RELEASED;
        }
        break;
    case BTN_STATE_RELEASED:
        if (!key && oldRealTicks >= BUTTONS_DOUBLECLICK_TIMEOUT) {
            btnState->state = BTN_STATE_IDLE;
            if (btnState->onClick) {
                btnState->onClick();
            }
        }
        else if (key) {
            btnState->state = BTN_STATE_ONCE_PRESSED;
            if (btnState->onDoubleClick) {
                btnState->onDoubleClick();
            }
        }
        break;
    case BTN_STATE_ONCE_PRESSED:
        if (!key) {
            btnState->state = BTN_STATE_IDLE;
        }
        break;
    }
}

void Buttons_UpdateTimers(void) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        UpdateButtonState(&states[i]);
    }
}
