#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_system.h"
#include "clock.h"
#include "buzzer.h"

#define DOTPOINT_MASK 0x80
static const uint16_t DIGIT_PORT_VALUES[] = {0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D,
                                             0x7D, 0x7, 0x7F, 0x6F};
static const uint16_t DIGIT_MASKS[] = {0x700, 0xB00, 0xD00, 0xE00};

static int clockTime = 0;
static int alarmTime = -1;
static int clockMode = CLOCKMODE_CLOCK;

static uint16_t currentDigits[] = {0, 0, 0, 0};

static void SetDigits(int time) {
    currentDigits[0] = time % 3600 / 60 % 10;
    currentDigits[1] = time % 3600 / 60 / 10;
    currentDigits[2] = time / 3600 % 10;
    currentDigits[3] = time / 3600 / 10;
}

void Clock_Init(void) {
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_OUTPUT);

    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_WriteOutputPort(GPIOC, 0x0000);
}

void Clock_Update(void) {
    static int digit = 0;

    int dotPoint = 0;
    if (clockMode == CLOCKMODE_CLOCK) {
        dotPoint = ((clockTime % 2 == 0) && (digit == 2)) ? 1 : 0;
    }
    else if (clockMode == CLOCKMODE_SETTIME) {
        dotPoint = (digit == 0) ? 1 : 0;
    }
    else if (clockMode == CLOCKMODE_SETALARM) {
        dotPoint = (digit == 1) ? 1 : 0;
    }

    if (clockMode == CLOCKMODE_SETTIME) {
        SetDigits(clockTime);
    }
    else if(clockMode == CLOCKMODE_SETALARM) {
        if (alarmTime == -1) {
            alarmTime = 0;
        }
        SetDigits(alarmTime);
    }

    LL_GPIO_WriteOutputPort(GPIOB,
                            DIGIT_PORT_VALUES[currentDigits[digit]] |
                            DIGIT_MASKS[digit] |
                            (dotPoint * DOTPOINT_MASK));
    digit = (digit + 1) % 4;
}

#define SECONDS_IN_DAY (3600 * 24)

void Clock_UpdateTimer(void) {
    static int tick = 0;
    tick++;
    if (tick == 1000) {
        tick = 0;

        if (clockMode == CLOCKMODE_SETTIME) {
            return;
        }

        if (clockMode != CLOCKMODE_SETALARM) {
            if (clockTime == alarmTime) {
                Buzzer_Buzz(BUZZ_ALARM);
            }
        }

        clockTime++;
        if (clockTime >= SECONDS_IN_DAY) {
            clockTime = 0;
        }

        SetDigits(clockTime);
    }
}

void Clock_SetMode(clockmode_t mode) {
    clockMode = mode;
}

clockmode_t Clock_GetMode(void) {
    return clockMode;
}

void Clock_AddClockTime(int t) {
    clockTime = (clockTime + t) % SECONDS_IN_DAY;
}
void Clock_AddAlarmTime(int t) {
    alarmTime = (alarmTime + t) % SECONDS_IN_DAY;
}

void Clock_ResetClockSeconds(void) {
    clockTime = clockTime - clockTime % 60;
}

void Clock_ResetAlarmSeconds(void) {
    alarmTime = alarmTime - alarmTime % 60;
}

void Clock_ResetAlarm(void) {
    alarmTime = -1;
}
