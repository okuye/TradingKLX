@protocol IO2GClosedTradeRow <IO2GRow>
    - (NSString*) getTradeID;
    - (NSString*) getAccountID;
    - (NSString*) getAccountName;
    - (NSString*) getAccountKind;
    - (NSString*) getOfferID;
    - (int) getAmount;
    - (NSString*) getBuySell;
    - (double) getGrossPL;
    - (double) getCommission;
    - (double) getRolloverInterest;
    - (double) getOpenRate;
    - (NSString*) getOpenQuoteID;
    - (DATE) getOpenTime;
    - (NSString*) getOpenOrderID;
    - (NSString*) getOpenOrderReqID;
    - (NSString*) getOpenOrderRequestTXT;
    - (NSString*) getOpenOrderParties;
    - (double) getCloseRate;
    - (NSString*) getCloseQuoteID;
    - (DATE) getCloseTime;
    - (NSString*) getCloseOrderID;
    - (NSString*) getCloseOrderReqID;
    - (NSString*) getCloseOrderRequestTXT;
    - (NSString*) getCloseOrderParties;
    - (NSString*) getTradeIDOrigin;
    - (NSString*) getTradeIDRemain;
    - (NSString*) getValueDate;
    - (double) getDividends;
    //
@end


@protocol IO2GClosedTradeTableRow <IO2GClosedTradeRow>
    - (double) getPL;
    - (NSString*) getInstrument;
    - (double) getNetPL;
@end
