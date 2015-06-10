#ifndef __keys_user_H
#define __keys_user_H

//////////////////////////////////////////////////////////////////////////////////////////////////
extern void  plus_alarm(uint32_t *param);
extern void minus_alarm(uint32_t *param);

extern void  plus_sleep(uint32_t *param);
extern void minus_sleep(uint32_t *param);

extern void  plus_on(uint32_t *param);
extern void minus_off(uint32_t *param);

extern void  plus_one(uint32_t *param);
extern void minus_one(uint32_t *param);

extern void  plus_ten(uint32_t *param);
extern void minus_ten(uint32_t *param);

extern void  plus_50(uint32_t *param);
extern void minus_50(uint32_t *param);

extern void  plus_500(uint32_t *param);
extern void minus_500(uint32_t *param);

void plus_doze_reset(uint32_t *param);
void plus_reboot(uint32_t *param);
void minus_poweroff(uint32_t *param);

void usb_activate(uint32_t *param);
void usb_deactivate(uint32_t *param);
	
//////////////////////////////////////////////////////////////////////////////////////////////////

void keys_proccessing(void);

#endif
