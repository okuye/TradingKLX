    typedef enum
    {
       IO2GTimeConverter_UTC,
       IO2GTimeConverter_Local,
       IO2GTimeConverter_EST,
       IO2GTimeConverter_Server
    } IO2GTimeConverter_TimeZone;

@protocol IO2GTimeConverter
    /** Converts the date and time from/to the UTC, EST, local and server time zones..
        @param dtSource         The date and time to convert.
        @param fromZone         The time zone to convert from..
        @param fromZone         The time zone to convert to. 
        @return                 The date and time in the time zone which you select in the OutputZone parameter..
    */
    - (DATE) convert: (DATE) dtSource :(IO2GTimeConverter_TimeZone) fromZone :(IO2GTimeConverter_TimeZone) toZone;
@end

