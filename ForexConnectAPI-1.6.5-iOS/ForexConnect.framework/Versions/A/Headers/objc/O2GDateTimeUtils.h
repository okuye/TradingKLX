#pragma once

#if !defined(SYSTEMTIME_DEFINED)
typedef struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#define SYSTEMTIME_DEFINED
#endif

enum O2GTimeZone
{
    Local,
    EST,
    UTC
};

@interface O2GDateTimeUtils : NSObject
+ (BOOL) CTimeToOleTime:(struct tm*) t :(double*) dt;
+ (void) CTimeToWindowsTime:(struct tm*) t :(SYSTEMTIME*) st;
+ (BOOL) OleTimeToWindowsTime:(double) dt :(SYSTEMTIME*) st;
+ (BOOL) OleTimeToCTime:(double) dt :(struct tm*) t;
+ (BOOL) WindowsTimeToOleTime:(SYSTEMTIME*) st :(double*) dt;
+ (void) WindowsTimeToCTime:(SYSTEMTIME*) st :(struct tm*) t;
+ (double) dateConvertTz:(double)date :(enum O2GTimeZone)fromTz :(enum O2GTimeZone)toTz;
@end

// O2GDateTimeUtils class redesigned for Swift
@interface O2GDateTimeUtils2 : NSObject
 + (double) parseOleTimeFromString:(NSString*)time;  // string format %m.%d.%Y %H:%M:%S
 + (double) OleTimeFromCTime:(struct tm)t;
 + (SYSTEMTIME) WindowsTimeFromCTime:(struct tm) t;
 + (SYSTEMTIME) WindowsTimeFromOleTime:(double) dt;
 + (struct tm) CTimeFromOleTime:(double) dt;
 + (double) OleTimeFromWindowsTime:(SYSTEMTIME) st;
 + (struct tm) CTimeFromWindowsTime:(SYSTEMTIME) st;
 + (NSDate*)oleTimeToCocoaTime:(double)oleDate;
 + (NSNumber*)cocoaTimeToOleTime:(NSDate*)date;
@end
