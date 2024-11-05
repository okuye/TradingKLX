@protocol IO2GMarketDataSnapshotResponseReader <IAddRef>
    - (BOOL) isBar;
    /** Gets a number market rate.*/
    - (int) size;
    /** Get the date and time of the begin of the period.*/
    - (DATE) getDate : (int) index;    
    /** Get the price of the bid tick.*/
    - (double) getBid : (int) index;
    /** Get the price of the ask tick.*/
    - (double) getAsk : (int) index;
    /** The open price of the bid bar or bid tick.*/
    - (double) getBidOpen : (int) index;
    /** Gets the highest prices of the bid bar.*/
    - (double) getBidHigh : (int) index;
    /** Gets the lowest price of the bid bar.*/
    - (double) getBidLow : (int) index;
    /** Gets the close price of the bid bar.*/
    - (double) getBidClose : (int) index;
    /** Gets the open price of the ask bar or ask tick.*/
    - (double) getAskOpen : (int) index;
    /** Gets the highest price of the ask bar.*/
    - (double) getAskHigh : (int) index;
    /** Gets the lowest price of the ask bar.*/
    - (double) getAskLow : (int) index;
    /** Gets the close price of the ask bar.*/
    - (double) getAskClose : (int) index;
    /** Gets the volume.*/
    - (int) getVolume : (int) index;
    /** Gets last bar volume. */
    - (int) getLastBarVolume;
    /** Gets last bar time.*/
    - (DATE) getLastBarTime;
@end
