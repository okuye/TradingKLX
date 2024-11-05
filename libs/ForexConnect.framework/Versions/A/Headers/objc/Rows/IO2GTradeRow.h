@protocol IO2GTradeRow <IO2GRow>
    - (NSString*) getTradeID;
    - (NSString*) getAccountID;
    - (NSString*) getAccountName;
    - (NSString*) getAccountKind;
    - (NSString*) getOfferID;
    - (int) getAmount;
    - (NSString*) getBuySell;
    - (double) getOpenRate;
    - (DATE) getOpenTime;
    - (NSString*) getOpenQuoteID;
    - (NSString*) getOpenOrderID;
    - (NSString*) getOpenOrderReqID;
    - (NSString*) getOpenOrderRequestTXT;
    - (double) getCommission;
    - (double) getRolloverInterest;
    - (NSString*) getTradeIDOrigin;
    - (NSString*) getValueDate;
    - (NSString*) getParties;
    - (double) getDividends;

    //

@end


@protocol IO2GTradeTableRow <IO2GTradeRow>
    - (double) getPL;
    - (double) getGrossPL;
    - (double) getClose;
    - (double) getStop;
    - (double) getLimit;
    - (NSString*) getStopOrderID;
    - (NSString*) getLimitOrderID;
    - (NSString*) getInstrument;
    - (double) getTrailRate;
    - (double) getTrailStep;
    - (double) getCloseCommission;
    - (double) getUsedMargin;

    //

@end

