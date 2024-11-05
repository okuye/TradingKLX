@protocol IO2GAccountRow <IO2GRow>
    - (NSString*) getAccountID;
    - (NSString*) getAccountName;
    - (NSString*) getAccountKind;
    - (double) getBalance;
    - (double) getNonTradeEquity;
    - (double) getM2MEquity;
    - (NSString*) getMarginCallFlag;
    - (DATE) getLastMarginCallDate;
    - (NSString*) getMaintenanceType;
    - (int) getAmountLimit;
    - (int) getBaseUnitSize;
    - (BOOL) getMaintenanceFlag;
    - (NSString*) getManagerAccountID;
    - (NSString*) getLeverageProfileID;
    - (double) getHedgeMarginPCT;
    - (NSString*) getATPID;
    - (NSString*) getARPID;

    //
@end

@protocol IO2GAccountTableRow <IO2GAccountRow>
    - (double) getEquity;
    - (double) getDayPL;
    - (double) getUsableMargin;
    - (double) getGrossPL;
    - (double) getUsedMargin;
    - (double) getUsedMargin3;
    - (int) getUsableMarginInPercentage;
    - (int) getUsableMaintMarginInPercentage;

    //
@end
