#include "../Includes/time.h"

static uint64_t system_time = 0; // System time in milliseconds
static RTC_Time last_rtc_time;

// Check if an RTC update is in progress
static int rtc_update_in_progress() {
    outPort(CMOS_ADDRESS, 0x0A);
    return inPort(CMOS_DATA) & 0x80;
}

// Read a specific CMOS register
static uint8_t read_cmos_register(uint8_t reg) {
    outPort(CMOS_ADDRESS, reg);
    return inPort(CMOS_DATA);
}

// Read RTC values into the provided RTC_Time struct
void read_rtc(RTC_Time* time) {
    while (rtc_update_in_progress()); // Wait for update to complete

    uint8_t century = 0;
    uint8_t registerB;

    time->second = read_cmos_register(0x00);
    time->minute = read_cmos_register(0x02);
    time->hour = read_cmos_register(0x04);
    time->day = read_cmos_register(0x07);
    time->month = read_cmos_register(0x08);
    time->year = read_cmos_register(0x09);
    registerB = read_cmos_register(0x0B);

    // Convert BCD to binary if needed
    if (!(registerB & 0x04)) {
        time->second = (time->second & 0x0F) + ((time->second / 16) * 10);
        time->minute = (time->minute & 0x0F) + ((time->minute / 16) * 10);
        time->hour = (time->hour & 0x0F) + ((time->hour / 16) * 10);
        time->day = (time->day & 0x0F) + ((time->day / 16) * 10);
        time->month = (time->month & 0x0F) + ((time->month / 16) * 10);
        time->year = (time->year & 0x0F) + ((time->year / 16) * 10);
    }

    // Compute full year
    time->year += (CURRENT_YEAR / 100) * 100;
    if (time->year < CURRENT_YEAR) {
        time->year += 100;
    }
}

// PIT interrupt updates system time
void pit_update_system_time(void) {
    system_time += 10; // PIT is configured for 10ms intervals
}

// Get the current system time in milliseconds
uint64_t get_system_time(void) {
    return system_time;
}

// Initialize the RTC
void init_rtc() {
    read_rtc(&last_rtc_time); // Read the initial RTC time
    system_time = 0; // Reset system time
}

// Synchronize system time with RTC
void synchronize_rtc_with_system_time(void) {
    RTC_Time current_time;
    read_rtc(&current_time);

    // Calculate system time based on the RTC time
    system_time = (uint64_t)current_time.hour * 3600000 +
                  (uint64_t)current_time.minute * 60000 +
                  (uint64_t)current_time.second * 1000;
}