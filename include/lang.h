#ifndef __lang_H
#define __lang_H

#ifndef LANG_EN
///////////////////////////////////////////////////////
// Menu
#define LANG_ALARM    "Тревога"
#define LANG_SLEEP    "Сон"
#define LANG_SOUND    "Звук"
#define LANG_VIBRO    "Вибро"
#define LANG_USBMODE  "Режим USB"
#define LANG_CLEARDO  "Сброс дозы"
#define LANG_REBOOT   "Перезаг/выкл"
#define LANG_CONTRAST "Контраст"
#define LANG_REVERSE  "Реверс"
#define LANG_COUNT    "Счет"
#define LANG_VOLTAGE  "Напряжение"
#define LANG_OFF      "откл"
#define LANG_QUARTZ   "Кварц"
#define LANG_ON       "вкл"
#define LANG_ALARM2   "трев"
#define LANG_USEC     "%uсек"
#define LANG_UMKR     "%uмкР"
#define LANG_UHZ      "%u Гц"
#define LANG_UV       "%uB"
#define LANG_V4PUMP   "Накачка"
#define LANG_UV4PUMP  "%uтик"
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Dose
#define LANG_MAXFON    "Максимум фона"
#define LANG_DOSE10M   "Доза за 10мин"
#define LANG_DOSEHOUR  "Доза за час"
#define LANG_DOSE24H   "Доза за сутки"
#define LANG_DOSEWEEK  "Доза за неделю"
#define LANG_DOSEMONTH "Доза за месяц"
#define LANG_DOSECALC  " *расчет* мкР"
#define LANG_9UMKR     "%9u мкР"
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Screen
#define LANG_MENU     "      Меню      "
#define LANG_STAT     "   Статистика   "
#define LANG_AKB3VVV  "АКБ  |+3В  |ВВ"
#define LANG_PUMP     "Накачка"
#define LANG_IMPMINAR "имп/м | наработ."
#define LANG_CALC2    "расчет"
#define LANG_4IDN     "%4iдн."

#define LANG_DALL     "Всего  %5i"
#define LANG_DIMP     "Имп.   %5i"
#define LANG_DTIME    "время %6i"
#define LANG_DSENS    "Датчик %5i"

#define LANG_ABOUT    "  О программе   "
#define LANG_DOZIK    "  Ультра-Микрон "
#define LANG_AUTHOR   "   (c)Shodan    "
#define LANG_CITY     " г.Тула 2014-15г"
#define LANG_SITE     " радиокружок.net"
#define LANG_BUILD    " Прошивка:"
///////////////////////////////////////////////////////

#define LANG_POWEROFF " Выключение.... "

#define LANG_REBOOTPR "Перезагрузка...."
#define LANG_DONTOTO1 "   На клавиши   "
#define LANG_DONTOTO2 "   НЕ нажимать! "
#define LANG_USBON    "USB активирован"
#define LANG_USBOFF   "USB  выключен"

#define LANG_UR       "мкР"
#define LANG_H        " ч "
///////////////////////////////////////////////////////

#else

///////////////////////////////////////////////////////
// Menu
#define LANG_ALARM    "Alarm"
#define LANG_SLEEP    "Sleep"
#define LANG_SOUND    "Sound"
#define LANG_VIBRO    "Vibro"
#define LANG_USBMODE  "USB state"
#define LANG_CLEARDO  "Clr dose"
#define LANG_REBOOT   "Reboot/off"
#define LANG_CONTRAST "Contrast"
#define LANG_REVERSE  "Reverse"
#define LANG_COUNT    "Count"
#define LANG_VOLTAGE  "Voltage"
#define LANG_ON       "on"
#define LANG_OFF      "off"
#define LANG_QUARTZ   "Quarz"
#define LANG_ALARM2   "alrm"
#define LANG_USEC     "%usec"
#define LANG_UMKR     "%uuRh"
#define LANG_UHZ      "%u Hz"
#define LANG_UV       "%uV"
#define LANG_V4PUMP   "Pump"
#define LANG_UV4PUMP  "%uT"
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Dose
#define LANG_MAXFON   "Radiation max"
#define LANG_DOSE10M  "10 min dose"
#define LANG_DOSEHOUR "Hour dose"
#define LANG_DOSE24H  "Day dose"
#define LANG_DOSEWEEK "Week dose"
#define LANG_DOSEMONTH "Month dose"
#define LANG_DOSECALC " *CALC*   uRh"
#define LANG_9UMKR    "%9u uRh"
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Screen
#define LANG_MENU     "      Menu      "
#define LANG_STAT     "   Statistic    "
#define LANG_ABOUT    "      About     "

#define LANG_AKB3VVV  "BAT  |+3V  |HV"
#define LANG_PUMP     "Pump  "
#define LANG_IMPMINAR "imp/m | uptime  "
#define LANG_CALC2    "calc  "
#define LANG_4IDN     "%4iday"

#define LANG_DALL     "All    %5i"
#define LANG_DIMP     "Imp.   %5i"
#define LANG_DTIME    "Time  %6i"
#define LANG_DSENS    "Sensor %5i"

#define LANG_DOZIK    "  Ultra-Micron  "
#define LANG_AUTHOR   " (c)Shodan,Tula "
#define LANG_CITY     " Russia 2014-15 "
#define LANG_SITE     "xn--h1aeegel.net"
#define LANG_BUILD    " FW Build:"
///////////////////////////////////////////////////////

#define LANG_POWEROFF "Power off...... "

#define LANG_REBOOTPR "Rebooting......."
#define LANG_DONTOTO1 "  DO NOT TOUCH  "
#define LANG_DONTOTO2 "  ANY KEYS!!!   "
#define LANG_USBON    "USB enabled"
#define LANG_USBOFF   "USB disabled"

#define LANG_UR       "uR"
#define LANG_H        " h "
///////////////////////////////////////////////////////


#endif
#endif
