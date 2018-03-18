#ifndef STM32CLOCK_BUZZER_H
#define STM32CLOCK_BUZZER_H

#define BUZZ_ALARM 10000
#define BUZZ_BUTTON 100
#define BUZZ_PERIOD 4

void Buzzer_Init(void);
void Buzzer_Buzz(int time);
void Buzzer_Stop(void);
void Buzzer_UpdateTimer(void);

#endif /* ifndef STM32CLOCK_BUZZER_H */
