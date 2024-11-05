
@protocol IO2GRolloverProvider, IO2GCommissionsProvider;

@protocol IO2GSessionDescriptor  <IAddRef>
    /** Gets the unique identifier of the descriptor. */
    - (NSString*) getID;
    /** Gets the readable name of the descriptor. */
    - (NSString*) getName;
    /** Gets the description of the descriptor. */
    - (NSString*) getDescription;

    - (BOOL) requiresPin;
@end

@protocol IO2GSessionDescriptorCollection <IAddRef>
    /** Gets number of session descriptors. */
    - (int) size;
    /** Gets the session descriptor by index.*/
    - (NSObject<IO2GSessionDescriptor>*) get: (int) index;
@end

typedef enum
{
    IO2GSession_Disconnected = 0,
    IO2GSession_Connecting = 1,
    IO2GSession_TradingSessionRequested = 2,
    IO2GSession_Connected = 3,
    IO2GSession_Reconnecting = 4,
    IO2GSession_Disconnecting = 5,
    IO2GSession_SessionLost = 6,
    IO2GSession_PriceSessionReconnecting = 7,
    IO2GSession_ConnectedWithNeedToChangePassword = 8,
    IO2GSession_ChartSessionReconnecting = 9,
} IO2GSessionStatus_O2GSessionStatus;


typedef enum
{
    IO2GChartSessionStatus_Disconnected = 0,
    IO2GChartSessionStatus_Connecting = 1,
    IO2GChartSessionStatus_Connected = 3,
    IO2GChartSessionStatus_Reconnecting = 4,
    IO2GChartSessionStatus_Disconnecting = 5,
    IO2GChartSessionStatus_SessionLost = 6,
} IO2GChartSessionStatus_O2GChartSessionStatus;

@protocol IO2GSessionStatus <IAddRef>
    - (void) onSessionStatusChanged: (IO2GSessionStatus_O2GSessionStatus) status;
    - (void) onLoginFailed: (NSString*) error;
@end

@protocol IO2GChartSessionStatus <IAddRef>
    - (void) onChartSessionStatusChanged:(IO2GChartSessionStatus_O2GChartSessionStatus) status;
    - (void) onChartSessionLoginFailed:(NSString*)error;
@end

@protocol IO2GSystemPropertiesListener <IAddRef>
    - (void) onChangeProperty:(NSString*)propertyName :(NSString*)propertyValue;
@end

@protocol IO2GTableManagerListener <IAddRef>
    - (void) onStatusChanged: (O2GTableManagerStatus) status :(id<IO2GTableManager>) tableManager;
@end

@protocol IO2GSession <IAddRef>
    - (NSObject<IO2GLoginRules>*) getLoginRules;
    - (IO2GSessionStatus_O2GSessionStatus) getSessionStatus;
    /** Establishes connection with the trade server.*/
    - (void) login: (NSString*) user :(NSString*) pwd :(NSString*) url :(NSString*) connection;
    /** Establishes connection with the trade server.*/
    - (void) loginWithToken: (NSString*) user :(NSString*) token :(NSString*) url :(NSString*) connection;
    /** Closes connection with the trade server.*/
    - (void) logout;
    /* Subscribes the session status listener.*/
    - (void) subscribeSessionStatus: (id<IO2GSessionStatus>) listener;
    /* Unsubscribes the session status listener.*/
    - (void) unsubscribeSessionStatus: (id<IO2GSessionStatus>) listener;
    /** Gets the session descriptors collection.*/
    - (NSObject<IO2GSessionDescriptorCollection>*) getTradingSessionDescriptors;
    /** Sets the trading session identifier and pin.*/
    - (void) setTradingSession: (NSString*) sessionId pin:(NSString*) pin;
    /** Subscribes response listener.*/
    - (void) subscribeResponse: (id<IO2GResponseListener>) listener;
    /** Unsubscribes response listener.*/
    - (void) unsubscribeResponse: (id<IO2GResponseListener>) listener;
    /** Subscribes system properties changes.*/
    - (void) subscribeSystemPropertiesChange:(id<IO2GSystemPropertiesListener>)listener;
    /** Unsubscribes system properties changes.*/
    - (void) unsubscribeSystemPropertiesChange:(id<IO2GSystemPropertiesListener>)listener;
    /* Subscribes the chart session status listener.*/
    - (void) subscribeChartSessionStatus:(id<IO2GChartSessionStatus>)listener;
    /* Unsubscribes the chart session status listener.*/
    - (void) unsubscribeChartSessionStatus:(id<IO2GChartSessionStatus>)listener;
    /** Get the request factory.*/
    - (NSObject<IO2GRequestFactory>*) getRequestFactory;
    /** Get the rollover provider.*/
    - (NSObject<IO2GRolloverProvider>*) getRolloverProvider;
    /** Get the commission provider.*/
    - (NSObject<IO2GCommissionsProvider>*) getCommissionsProvider;
    /** Gets the response factory reader.*/
    - (NSObject<IO2GResponseReaderFactory>*) getResponseReaderFactory;
    /** Send the request to the trade server.*/
    - (void) sendRequest: (id<IO2GRequest>) request;
    /** Gets time converter for converting request and markes snapshot date.*/
    - (NSObject<IO2GTimeConverter>*) getTimeConverter;
    /** Set session mode.*/
    - (void) setPriceUpdateMode: (O2GPriceUpdateMode) mode;
    /** Get session mode.*/
    - (O2GPriceUpdateMode) getPriceUpdateMode;    
    /** Get server time.*/
    - (DATE) getServerTime;
    /** Get table manager.*/
    - (NSObject<IO2GTableManager>*) getTableManager;
    /** Get table manager by account.*/
    - (NSObject<IO2GTableManager>*) getTableManagerByAccount: (NSString*) accountID;
    /** Set how to use table manager.*/
    - (void) useTableManager: (O2GTableManagerMode) mode :(id<IO2GTableManagerListener>) tablesListener;
    /** Set chart session mode.*/
    - (void) setChartSessionMode: (O2GChartSessionMode) mode;
    /** Get chart session mode.*/
    - (O2GChartSessionMode) getChartSessionMode;
    /** Set timeout(ms) for all requests. */
    - (void)setRequestsTimeout: (unsigned int)timeout;
    /** Get timeout(ms) for all requests. */
    - (unsigned int)getRequestsTimeout;
    /** Set price refresh rate (ms).*/
    - (BOOL) setPriceRefreshRate: (int)priceRefreshRate;
    /** Get price refresh rate (ms).*/
    - (int) getPriceRefreshRate;
    /** Get minimum price refresh rate (ms).*/
    - (int) getMinPriceRefreshRate;
    /** Get maximum price refresh rate (ms).*/
    - (int) getMaxPriceRefreshRate;
    /** Gets session sub ID. */
    - (NSString*) getSessionSubID;
    /** Gets current user kind*/
    - (O2GUserKind) getUserKind;
    /** Gets user name. */
    - (NSString*) getUserName;

    /** Gets report URL.
     
     @param  account         Account.
     @param  dateFrom        Start date of the report period.
     @param  dateTo          End date of the report period.
     @param  format          Report format. HTML will be used in case of 0.
     @param  langID          Language and locale of the report.
     
     @return URL string. Nil indicates failure.
     */
    - (NSString*)getReportURL:(NSString*)accountID dateFrom:(double)dateFrom dateTo:(double)dateTo format:(NSString*)format langID:(NSString*)langID;

    /** Gets token
          
     @return token string
     */
    - (NSString*)getToken;

@end


















