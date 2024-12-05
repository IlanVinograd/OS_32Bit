#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>
#include <io.h>

// CMOS I/O ports
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CURRENT_YEAR 2023 // Update this each year

// RTC time structure
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} RTC_Time;

void init_rtc();
void read_rtc(RTC_Time* time);
void pit_update_system_time(void);
uint64_t get_system_time(void); // Returns system time in milliseconds
void synchronize_rtc_with_system_time(void);

#endif /* _TIME_H_ */