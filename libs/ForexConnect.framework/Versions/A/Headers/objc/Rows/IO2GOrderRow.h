
typedef NS_ENUM(NSInteger, IO2GOrderRow_O2GRateType) {
    IO2GOrderRow_RateNone = 0,
    IO2GOrderRow_RateNonPegged = 1,
    IO2GOrderRow_RatePeggedOpen = 2,
    IO2GOrderRow_RatePeggedClose = 3
};

typedef NS_ENUM(NSInteger, IO2GOrderRow_O2GContingencyType) {
    IO2GOrderRow_ContingencyTypeNo = 0,
    IO2GOrderRow_ContingencyTypeOCO = 1,
    IO2GOrderRow_ContingencyTypeOTO = 2,
    IO2GOrderRow_ContingencyTypeELS = 3,
    IO2GOrderRow_ContingencyTypeOTOCO = 4,
};

@protocol IO2GOrderRow <IO2GRow>
    - (NSString*) getOrderID;
    - (NSString*) getRequestID;
    - (double) getRate;
    - (double) getExecutionRate;
    - (double) getRateMin;
    - (double) getRateMax;
    - (NSString*) getTradeID;
    - (NSString*) getAccountID;
    - (NSString*) getAccountName;
    - (NSString*) getOfferID;
    - (BOOL) getNetQuantity;
    - (NSString*) getBuySell;
    - (NSString*) getStage;
    - (NSString*) getType;
    - (NSString*) getStatus;
    - (DATE) getStatusTime;
    - (int) getAmount;
    - (double) getLifetime;
    - (double) getAtMarket;
    - (int) getTrailStep;
    - (double) getTrailRate;
    - (NSString*) getTimeInForce;
    - (NSString*) getAccountKind;
    - (NSString*) getRequestTXT;
    - (NSString*) getContingentOrderID;
    - (IO2GOrderRow_O2GContingencyType) getContingencyType;
    - (NSString*) getPrimaryID;
    - (int) getOriginAmount;
    - (int) getFilledAmount;
    - (BOOL) getWorkingIndicator;
    - (NSString*) getPegType;
    - (double) getPegOffset;
    - (DATE) getExpireDate;
    - (NSString*) getValueDate;
    - (NSString*) getParties;
    - (double) getPegOffsetMin;
    - (double) getPegOffsetMax;
    - (int) getSide;
    //

@end


@protocol IO2GOrderTableRow <IO2GOrderRow>
    - (double) getStop;
    - (double) getLimit;
    - (NSString*) getStopOrderID;
    - (NSString*) getLimitOrderID;
    - (IO2GOrderRow_O2GRateType) getTypeStop;
    - (IO2GOrderRow_O2GRateType) getTypeLimit;
    - (int) getStopTrailStep;
    - (double) getStopTrailRate;
    //

@end

