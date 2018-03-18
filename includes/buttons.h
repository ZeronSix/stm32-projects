#ifndef STM32CLOCK_BUTTONS_H
#define STM32CLOCK_BUTTONS_H

#define BUTTONS_SWITCH_TIMEOUT 10
#define BUTTONS_DOUBLECLICK_TIMEOUT 100

enum {
    BUTTON_SETTIME,
    BUTTON_INC,
    BUTTON_ALARM_RESET,
    BUTTON_COUNT,
};

void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void Buttons_Init(void);
void Buttons_UpdateTimers(void);

#endif /* ifndef STM32CLOCK_BUTTONS_H */
