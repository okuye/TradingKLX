
#ifndef __cplusplus
typedef enum 
{
    Tick = 0,                    //!< tick
    Min = 1,                    //!< 1 minute    
    Hour = 2,                   //!< 1 hour
    Day = 3,                    //!< 1 day
    Week = 4,                   //!< 1 week
    Month = 5,                  //!< 1 month
    Year = 6   
} O2GTimeframeUnit;
#endif

/** */
@protocol IO2GTimeframe <IAddRef>
    /** Gets the unique identifier of the timeframe.*/
    - (NSString*) getID;

    - (O2GTimeframeUnit) getUnit;

    - (int)getQueryDepth;

    /** Gets number of */
    - (int) getSize;
@end


@protocol IO2GTimeframeCollection <IAddRef>
    /** Get numbers of the time frames.*/
    - (int) size;
    /** Gets time frame by the index.*/
    - (NSObject<IO2GTimeframe>*) getByIndex: (int) index;
    /** Gets time frame by the unique id.*/
    - (NSObject<IO2GTimeframe>*) getByID: (NSString*) ID;
@end
