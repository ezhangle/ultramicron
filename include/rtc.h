#ifndef __rtc_user_H
#define __rtc_user_H

void RTC_TimeRegulate(void);
void RTC_Config(void);
void RTC_Alarm_IRQHandler(void);
void init_rtc(void);
void Set_next_alarm_wakeup(void);

#endif
