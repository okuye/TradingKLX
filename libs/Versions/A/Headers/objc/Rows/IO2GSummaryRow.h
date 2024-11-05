@protocol IO2GSummaryRow <IO2GRow>
@end


@protocol IO2GSummaryTableRow <IO2GSummaryRow>
    - (NSString*) getOfferID;
    - (int) getDefaultSortOrder;
    - (NSString*) getInstrument;
    - (double) getSellNetPL;
    - (double) getSellAmount;
    - (double) getSellAvgOpen;
    - (double) getBuyClose;
    - (double) getSellClose;
    - (double) getBuyAvgOpen;
    - (double) getBuyAmount;
    - (double) getBuyNetPL;
    - (double) getAmount;
    - (double) getGrossPL;
    - (double) getNetPL;
    - (double) getSellNetPLPip;
    - (double) getBuyNetPLPip;
    - (double) getRolloverInterestSum;
    - (double) getUsedMargin;
    - (double) getUsedMarginBuy;
    - (double) getUsedMarginSell;
    - (double) getCommission;
    - (double) getCloseCommission;
    - (double) getDividends;

    //

@end

