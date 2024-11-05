
@protocol IO2GResponse <IAddRef>
    - (O2GResponseType) getType;
    - (NSString*) getRequestID;
@end


@protocol IO2GResponseListener <IAddRef>
    - (void) onRequestCompleted:(NSString*) requestId :(id<IO2GResponse>) response;
    - (void) onRequestFailed: (NSString*)requestId :(NSString*) error;
    - (void) onTablesUpdates: (id<IO2GResponse>) tablesUpdates;
@end


@protocol IO2GResponseReaderFactory <IAddRef>
    /** Create table updates response reader.*/
    - (NSObject<IO2GTablesUpdatesReader>*) createTablesUpdatesReader :(id<IO2GResponse>) response;
    
    /** Create market data snapshot response.*/
    - (NSObject<IO2GMarketDataSnapshotResponseReader>*) createMarketDataSnapshotReader :(id<IO2GResponse>) response;     
    
    /** Create offers table response reader.*/
    - (NSObject<IO2GOffersTableResponseReader>*) createOffersTableReader :(id<IO2GResponse>) response ;
    
    /** Create Account table response reader.*/  
    - (NSObject<IO2GAccountsTableResponseReader>*) createAccountsTableReader :(id<IO2GResponse>) response ;
    
    /** Create Order table response reader.*/  
    - (NSObject<IO2GOrdersTableResponseReader>*) createOrdersTableReader :(id<IO2GResponse>) response ;
    
    /** Create trades table response reader.*/  
    - (NSObject<IO2GTradesTableResponseReader>*) createTradesTableReader :(id<IO2GResponse>) response ;
    
    /** Create closed trades table response reader.*/  
    - (NSObject<IO2GClosedTradesTableResponseReader>*) createClosedTradesTableReader :(id<IO2GResponse>) response ;
    
    /** Create messages table response reader.*/  
    - (NSObject<IO2GMessagesTableResponseReader>*) createMessagesTableReader :(id<IO2GResponse>) response ;
    
    /** Create order response reader.*/
    - (NSObject<IO2GOrderResponseReader>*) createOrderResponseReader :(id<IO2GResponse>) response ; 
    
    /** Create system properties reader.*/
    - (NSObject<IO2GSystemPropertiesReader>*) createSystemPropertiesReader :(id<IO2GResponse>) response;

    /** Create market data response.*/
    - (NSObject<IO2GMarketDataResponseReader>*) createMarketDataReader :(id<IO2GResponse>)response;

    /** Create level 2 market data response reader. */
    - (NSObject<IO2GLevel2MarketDataUpdatesReader>*) createLevel2MarketDataReader :(id<IO2GResponse>)response;

    /** Create last order update response reader. */
    - (NSObject<IO2GLastOrderUpdateResponseReader>*) createLastOrderUpdateResponseReader :(id<IO2GResponse>)response;

    /** Process margin requirements response.*/
    - (BOOL) processMarginRequirementsResponse :(id<IO2GResponse>) response;
@end


















