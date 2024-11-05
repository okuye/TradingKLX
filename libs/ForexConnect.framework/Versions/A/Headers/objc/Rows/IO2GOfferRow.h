typedef NS_ENUM(NSInteger, IO2GOfferRow_O2GInstrumentType) {
    IO2GOfferRow_InstrumentForex = 1,        //!< Regular forex instrument.
    IO2GOfferRow_InstrumentIndices = 2,      //!< Indices.
    IO2GOfferRow_InstrumentCommodity = 3,    //!< Commodities.
    IO2GOfferRow_InstrumentTreasury = 4,     //!< Treasuries.
    IO2GOfferRow_InstrumentBullion = 5,      //!< Bullion.
    IO2GOfferRow_InstrumentShares = 6,       //!< Shares.
    IO2GOfferRow_InstrumentFXIndex = 7,      //!< FXIndex.
    IO2GOfferRow_InstrumentCFDShares = 8,    //!< CFD Shares.
    IO2GOfferRow_InstrumentCrypto = 9        //!< Crypto.
};

@protocol IO2GOfferRow <IO2GRow>
    - (NSString*) getOfferID;
    - (NSString*) getInstrument;
    - (NSString*) getQuoteID;
    - (double) getBid;
    - (double) getAsk;
    - (double) getLow;
    - (double) getHigh;
    - (int) getVolume;
    - (DATE) getTime;
    - (NSString*) getBidTradable;
    - (NSString*) getAskTradable;
    - (double) getSellInterest;
    - (double) getBuyInterest;
    - (NSString*) getContractCurrency;
    - (int) getDigits;
    - (double) getPointSize;
    - (NSString*) getSubscriptionStatus;
    - (IO2GOfferRow_O2GInstrumentType) getInstrumentType;
    - (int) getContractMultiplier;
    - (NSString*) getTradingStatus;
    - (NSString*) getValueDate;
    - (NSString*) getBidID;
    - (NSString*) getAskID;
    - (DATE) getBidExpireDate;
    - (DATE) getAskExpireDate;
    - (double) getDividendSell;
    - (double) getDividendBuy;

    - (BOOL) isOfferIDValid;
    - (BOOL) isInstrumentValid;
    - (BOOL) isQuoteIDValid;
    - (BOOL) isBidValid;
    - (BOOL) isAskValid;
    - (BOOL) isLowValid;
    - (BOOL) isHighValid;
    - (BOOL) isVolumeValid;
    - (BOOL) isTimeValid;
    - (BOOL) isBidTradableValid;
    - (BOOL) isAskTradableValid;
    - (BOOL) isSellInterestValid;
    - (BOOL) isBuyInterestValid;
    - (BOOL) isContractCurrencyValid;
    - (BOOL) isDigitsValid;
    - (BOOL) isPointSizeValid;
    - (BOOL) isSubscriptionStatusValid;
    - (BOOL) isInstrumentTypeValid;
    - (BOOL) isContractMultiplierValid;
    - (BOOL) isTradingStatusValid;
    - (BOOL) isValueDateValid;
    - (BOOL) isBidIDValid;
    - (BOOL) isAskIDValid;
    - (BOOL) isBidExpireDateValid;
    - (BOOL) isAskExpireDateValid;
    - (BOOL) isDividendSellValid;
    - (BOOL) isDividendBuyValid;

@end

@protocol IO2GOfferTableRow <IO2GOfferRow>
    - (double) getPipCost;
    - (int) getBidChangeDirection;
    - (int) getAskChangeDirection;
    - (int) getHiChangeDirection;
    - (int) getLowChangeDirection;
    - (int) getDefaultSortOrder;
    - (int) getFractionalPipSize;
    - (BOOL) isBidChangeDirectionValid;
    - (BOOL) isAskChangeDirectionValid;
    - (BOOL) isHiChangeDirectionValid;
    - (BOOL) isLowChangeDirectionValid;
    - (BOOL) isDefaultSortOrderValid;
    - (BOOL) isFractionalPipSizeValid;


@end

