/**
 * @file rtc_manager.h
 * @brief Real-Time Clock (RTC) management wrapper for N32G031K8
 * 
 * Provides high-level functions for:
 * - Time and date management
 * - Alarm configuration (Alarm A and Alarm B)
 * - Wakeup timer
 * - Timestamp capture
 * - Calibration and interrupt handling
 */

#ifndef __RTC_MANAGER_H__
#define __RTC_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Time format selection
 */
typedef enum {
    RTCMGR_FORMAT_BIN = 0,     /* Binary format */
    RTCMGR_FORMAT_BCD = 1      /* BCD format */
} RTCMgr_Format_t;

/**
 * @brief Hour format selection
 */
typedef enum {
    RTCMGR_HOUR_FORMAT_24 = 0, /* 24-hour format */
    RTCMGR_HOUR_FORMAT_12 = 1  /* 12-hour format (AM/PM) */
} RTCMgr_HourFormat_t;

/**
 * @brief AM/PM designation for 12-hour format
 */
typedef enum {
    RTCMGR_AM = 0,
    RTCMGR_PM = 1
} RTCMgr_AMPM_t;

/**
 * @brief Day of week
 */
typedef enum {
    RTCMGR_WEEKDAY_MONDAY    = 1,
    RTCMGR_WEEKDAY_TUESDAY   = 2,
    RTCMGR_WEEKDAY_WEDNESDAY = 3,
    RTCMGR_WEEKDAY_THURSDAY  = 4,
    RTCMGR_WEEKDAY_FRIDAY    = 5,
    RTCMGR_WEEKDAY_SATURDAY  = 6,
    RTCMGR_WEEKDAY_SUNDAY    = 7
} RTCMgr_Weekday_t;

/**
 * @brief RTC time structure
 */
typedef struct {
    uint8_t hours;          /* Hours (0-23 for 24h, 1-12 for 12h) */
    uint8_t minutes;        /* Minutes (0-59) */
    uint8_t seconds;        /* Seconds (0-59) */
    uint8_t sub_seconds;    /* Sub-seconds (0-999 for milliseconds) */
    RTCMgr_AMPM_t am_pm;    /* AM/PM (only for 12-hour format) */
    RTCMgr_Format_t format; /* Binary or BCD format */
} RTCMgr_Time_t;

/**
 * @brief RTC date structure
 */
typedef struct {
    uint8_t year;              /* Year (0-99, represents 2000-2099) */
    uint8_t month;             /* Month (1-12) */
    uint8_t day;               /* Day (1-31) */
    RTCMgr_Weekday_t weekday;  /* Day of week */
    RTCMgr_Format_t format;    /* Binary or BCD format */
} RTCMgr_Date_t;

/**
 * @brief RTC alarm structure
 */
typedef struct {
    RTCMgr_Time_t time;     /* Alarm time */
    uint8_t day;            /* Alarm day (1-31) or weekday */
    bool day_is_weekday;    /* true: day field is weekday, false: day field is date */
    bool mask_day;          /* Ignore day field in comparison */
    bool mask_hours;        /* Ignore hours field in comparison */
    bool mask_minutes;      /* Ignore minutes field in comparison */
    bool mask_seconds;      /* Ignore seconds field in comparison */
} RTCMgr_Alarm_t;

/**
 * @brief RTC alarm selection
 */
typedef enum {
    RTCMGR_ALARM_A = 0,
    RTCMGR_ALARM_B = 1
} RTCMgr_AlarmType_t;

/**
 * @brief RTC wakeup timer clock source
 */
typedef enum {
    RTCMGR_WAKEUP_CLOCK_RTCCLK_DIV16   = 0,  /* RTC/16 clock */
    RTCMGR_WAKEUP_CLOCK_RTCCLK_DIV8    = 1,  /* RTC/8 clock */
    RTCMGR_WAKEUP_CLOCK_RTCCLK_DIV4    = 2,  /* RTC/4 clock */
    RTCMGR_WAKEUP_CLOCK_RTCCLK_DIV2    = 3,  /* RTC/2 clock */
    RTCMGR_WAKEUP_CLOCK_1HZ            = 4,  /* 1Hz clock with counter 0x0000-0xFFFF */
    RTCMGR_WAKEUP_CLOCK_1HZ_PLUS_65536 = 6   /* 1Hz clock with counter 0x10000-0x1FFFF */
} RTCMgr_WakeupClock_t;

/**
 * @brief RTC interrupt types
 */
typedef enum {
    RTCMGR_INT_ALARM_A    = 0x01,
    RTCMGR_INT_ALARM_B    = 0x02,
    RTCMGR_INT_WAKEUP     = 0x04,
    RTCMGR_INT_TIMESTAMP  = 0x08
} RTCMgr_Interrupt_t;

/**
 * @brief Initialize RTC with LSE or LSI clock
 * Configures RTC clock source, enables RTC, and sets default time/date if needed.
 * @param use_lse true to use LSE (32.768kHz crystal), false to use LSI (~32kHz internal)
 * @return true if initialization successful, false otherwise
 */
bool RTCManager_Init(bool use_lse);

/**
 * @brief Deinitialize RTC
 * Disables RTC clock and resets configuration
 */
void RTCManager_DeInit(void);

/**
 * @brief Set current time
 * @param time Pointer to time structure (BIN or BCD format)
 * @param hour_format 12-hour or 24-hour format
 * @return true if successful, false otherwise
 */
bool RTCManager_SetTime(RTCMgr_Time_t *time, RTCMgr_HourFormat_t hour_format);

/**
 * @brief Get current time
 * @param time Pointer to time structure to fill
 * @return true if successful, false otherwise
 */
bool RTCManager_GetTime(RTCMgr_Time_t *time);

/**
 * @brief Set current date
 * @param date Pointer to date structure (BIN or BCD format)
 * @return true if successful, false otherwise
 */
bool RTCManager_SetDate(RTCMgr_Date_t *date);

/**
 * @brief Get current date
 * @param date Pointer to date structure to fill
 * @return true if successful, false otherwise
 */
bool RTCManager_GetDate(RTCMgr_Date_t *date);

/**
 * @brief Set alarm
 * @param alarm_type Alarm A or Alarm B
 * @param alarm Pointer to alarm configuration
 * @return true if successful, false otherwise
 */
bool RTCManager_SetAlarm(RTCMgr_AlarmType_t alarm_type, RTCMgr_Alarm_t *alarm);

/**
 * @brief Get alarm configuration
 * @param alarm_type Alarm A or Alarm B
 * @param alarm Pointer to alarm structure to fill
 * @return true if successful, false otherwise
 */
bool RTCManager_GetAlarm(RTCMgr_AlarmType_t alarm_type, RTCMgr_Alarm_t *alarm);

/**
 * @brief Enable alarm
 * @param alarm_type Alarm A or Alarm B
 * @param enable true to enable, false to disable
 */
void RTCManager_EnableAlarm(RTCMgr_AlarmType_t alarm_type, bool enable);

/**
 * @brief Configure wakeup timer
 * Sets the wakeup timer to trigger at specified intervals.
 * @param clock_source Wakeup timer clock source
 * @param counter Counter value (0-0xFFFF for most clocks, 0-0x1FFFF for 1Hz modes)
 * @return true if successful, false otherwise
 */
bool RTCManager_SetWakeupTimer(RTCMgr_WakeupClock_t clock_source, uint32_t counter);

/**
 * @brief Enable/disable wakeup timer
 * @param enable true to enable, false to disable
 */
void RTCManager_EnableWakeupTimer(bool enable);

/**
 * @brief Enable RTC interrupts
 * @param interrupts Bitmask of RTCMgr_Interrupt_t values
 */
void RTCManager_EnableInterrupt(uint32_t interrupts);

/**
 * @brief Disable RTC interrupts
 * @param interrupts Bitmask of RTCMgr_Interrupt_t values
 */
void RTCManager_DisableInterrupt(uint32_t interrupts);

/**
 * @brief Check if interrupt flag is set
 * @param interrupt RTC interrupt type
 * @return true if flag is set, false otherwise
 */
bool RTCManager_GetInterruptFlag(RTCMgr_Interrupt_t interrupt);

/**
 * @brief Clear interrupt flag
 * @param interrupt RTC interrupt type
 */
void RTCManager_ClearInterruptFlag(RTCMgr_Interrupt_t interrupt);

/**
 * @brief Convert BCD to Binary
 * @param bcd BCD value (0x00-0x99)
 * @return Binary value (0-99)
 */
uint8_t RTCManager_BCD2BIN(uint8_t bcd);

/**
 * @brief Convert Binary to BCD
 * @param bin Binary value (0-99)
 * @return BCD value (0x00-0x99)
 */
uint8_t RTCManager_BIN2BCD(uint8_t bin);

/**
 * @brief Get Unix timestamp (seconds since 1970-01-01 00:00:00)
 * @return Unix timestamp
 */
uint32_t RTCManager_GetUnixTimestamp(void);

/**
 * @brief Set time from Unix timestamp
 * @param timestamp Unix timestamp (seconds since 1970-01-01 00:00:00)
 * @return true if successful, false otherwise
 */
bool RTCManager_SetUnixTimestamp(uint32_t timestamp);

/**
 * @brief Calculate day of week from date (Zeller's congruence)
 * @param year Year (2000-2099)
 * @param month Month (1-12)
 * @param day Day (1-31)
 * @return Day of week (1=Monday, 7=Sunday)
 */
RTCMgr_Weekday_t RTCManager_CalculateWeekday(uint16_t year, uint8_t month, uint8_t day);

/**
 * @brief Check if year is leap year
 * @param year Year (2000-2099)
 * @return true if leap year, false otherwise
 */
bool RTCManager_IsLeapYear(uint16_t year);

/**
 * @brief Get days in month
 * @param year Year (2000-2099)
 * @param month Month (1-12)
 * @return Number of days in month (28-31)
 */
uint8_t RTCManager_GetDaysInMonth(uint16_t year, uint8_t month);

/**
 * @brief Format time as string "HH:MM:SS"
 * @param time Pointer to time structure
 * @param buffer Output buffer (minimum 9 bytes)
 */
void RTCManager_FormatTime(RTCMgr_Time_t *time, char *buffer);

/**
 * @brief Format date as string "YYYY-MM-DD"
 * @param date Pointer to date structure
 * @param buffer Output buffer (minimum 11 bytes)
 */
void RTCManager_FormatDate(RTCMgr_Date_t *date, char *buffer);

/**
 * @brief Format datetime as ISO 8601 string "YYYY-MM-DDTHH:MM:SS"
 * @param date Pointer to date structure
 * @param time Pointer to time structure
 * @param buffer Output buffer (minimum 20 bytes)
 */
void RTCManager_FormatDateTime(RTCMgr_Date_t *date, RTCMgr_Time_t *time, char *buffer);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_MANAGER_H__ */
