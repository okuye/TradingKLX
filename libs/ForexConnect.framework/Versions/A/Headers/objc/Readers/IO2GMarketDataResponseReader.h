@protocol IO2GMarketDataResponseReader <IAddRef>

    /** Gets QuoteID.*/
    - (NSString*) getQuoteID;
    /** Gets Instrument.*/
    - (NSString*) getInstrument;
    /** Gets Symbol ID (i.e. Offer ID).*/
    - (int) getSymbolID;
    /** Gets DateTime.*/
    - (DATE) getDateTime;

    /** Gets AskLow.*/
    - (double) getAskLow;
    /** Gets AskHigh.*/
    - (double) getAskHigh;
    /** Gets AskOpen.*/
    - (double) getAskOpen;
    /** Gets AskClose.*/
    - (double) getAskClose;

    /** Gets BidLow.*/
    - (double) getBidLow;
    /** Gets BidHigh.*/
    - (double) getBidHigh;
    /** Gets BidOpen.*/
    - (double) getBidOpen;
    /** Gets BidClose.*/
    - (double) getBidClose;

    /** Gets Low.*/
    - (double) getLow;
    /** Gets High.*/
    - (double) getHigh;

    /** Gets Timing Interval.*/
    - (int) getTimingInterval;
    /** Gets True if candle is completed, overwise False.*/
    - (BOOL) isCandleCompleted;
    /** Gets Market Data Request ID.*/
    - (NSString*) getMarketDataRequestID;
    /** Gets Trading Session ID.*/
    - (NSString*) getTradingSessionID;
    /** Gets Trading Session sub ID.*/
    - (NSString*) getTradingSessionSubID;
    /** Gets Continuous Flag.*/
    - (int) getContinuosFlag;

    /** Gets Bid ID.*/
    - (NSString*) getBidID;
    /** Gets Bid Quote Condition.*/
    - (NSString*) getBidQuoteCondition;
    /** Gets Bid Quote Type.*/
    - (int) getBidQuoteType;
    /** Gets Bid Expire Date Time.*/
    - (DATE) getBidExpireDateTime;

    /** Gets Ask ID.*/
    - (NSString*) getAskID;
    /** Gets Ask Quote Condition.*/
    - (NSString*) getAskQuoteCondition;
    /** Gets Ask Quote Type.*/
    - (int) getAskQuoteType;
    /** Gets Ask Expire Date Time.*/
    - (DATE) getAskExpireDateTime;

@end
