#ifndef __power_user_H
#define __power_user_H

#define BOR_LEVEL OB_BOR_OFF  /*!< BOR is disabled at power down, the reset is asserted when the VDD power supply reachs the PDR(Power Down Reset) threshold (1.5V)*/
//#define BOR_LEVEL OB_BOR_LEVEL1  /*!< BOR Reset threshold levels for 1.7V - 1.8V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL2  /*!< BOR Reset threshold levels for 1.9V - 2.0V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL3  /*!< BOR Reset threshold levels for 2.3V - 2.4V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL4  /*!< BOR Reset threshold levels for 2.55V - 2.65V VDD power supply */
//#define BOR_LEVEL OB_BOR_LEVEL5  /*!< BOR Reset threshold levels for 2.8V - 2.9V VDD power supply */




void set_bor(void);

#endif
