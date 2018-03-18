#ifndef STM32CLOCK_CLOCK_H
#define STM32CLOCK_CLOCK_H

typedef enum {
    CLOCKMODE_CLOCK,
    CLOCKMODE_SETTIME,
    CLOCKMODE_SETALARM
} clockmode_t;

void Clock_Init(void);
void Clock_Update(void);
void Clock_UpdateTimer(void);
void Clock_SetMode(clockmode_t mode);
clockmode_t Clock_GetMode(void);
void Clock_AddClockTime(int t);
void Clock_AddAlarmTime(int t);
void Clock_ResetClockSeconds(void);
void Clock_ResetAlarmSeconds(void);
void Clock_ResetAlarm(void);

#endif /* ifndef STM32CLOCK_CLOCK_H */
