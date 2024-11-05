
@protocol IO2GRequest <IAddRef>
    - (NSString*) getRequestID;
    - (int) getChildrenCount; 
    - (NSObject<IO2GRequest>*) getChildRequest: (int) index;
@end

@protocol IO2GValueMap <IAddRef>
    - (void) setString : (O2GRequestParamsEnum) param :(NSString*) value;
    - (void) setDouble : (O2GRequestParamsEnum) param :(double) value;
    - (void) setInt : (O2GRequestParamsEnum) param :(int) value; 
    - (void) setBoolean : (O2GRequestParamsEnum) param :(BOOL) value; 

    - (NSObject<IO2GValueMap>*) clone;  
    - (void) clear;
    
    - (int) getChildrenCount;
    - (NSObject<IO2GValueMap>*) getChild: (int) index;
    - (void) appendChild: (id<IO2GValueMap>) valueMap;
@end

/** Request factory.*/
@protocol IO2GRequestFactory <IAddRef>
    /** Gets time frames collection.*/
    - (NSObject<IO2GTimeframeCollection>*) getTimeFrameCollection;

    /** Create market a data snapshot request.*/
    - (NSObject<IO2GRequest>*) createMarketDataSnapshotRequestInstrument: (NSString*)instrument :(id<IO2GTimeframe>)timeframe :(int) maxBars;

    /** Fill parameters the market data snapshot*/
    - (void) fillMarketDataSnapshotRequestTime: (id<IO2GRequest>) request :(DATE) timeFrom :(DATE) timeTo :(BOOL) isIncludeWeekends :(O2GCandleOpenPriceMode) mode;

    /** Gets refresh request.*/
    - (NSObject<IO2GRequest>*) createRefreshTableRequest: (O2GTable) table;

    /** Create "confirmation mail" request*/
    - (NSObject<IO2GRequest>*) createConfirmationMailRequest: (id<IO2GMessageRow>) row;

    /** Gets refresh request by the account.*/
    - (NSObject<IO2GRequest>*) createRefreshTableRequestByAccount: (O2GTable) table :(NSString*) account;

    /** Create order request.*/
    - (NSObject<IO2GRequest>*) createOrderRequest: (id<IO2GValueMap>) valueMap;
    
    /** Create value map.*/
    - (NSObject<IO2GValueMap>*) createValueMap; 

    /** Get last error.*/
    - (NSString*) getLastError;
@end
