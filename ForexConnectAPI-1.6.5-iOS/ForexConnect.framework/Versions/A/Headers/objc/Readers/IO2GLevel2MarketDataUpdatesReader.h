@protocol IO2GLevel2MarketDataUpdatesReader <IAddRef>

- (int) getPriceQuotesCount;
- (double) getDateTime:(int) quoteIdx;
- (int) getSymbolID:(int) quoteIdx;
- (int) getVolume:(int) quoteIdx;

- (int) getPricesCount:(int) quoteIdx;
- (BOOL)isBid:(int) quoteIdx :(int) priceIdx;
- (BOOL) isAsk:(int) quoteIdx :(int) priceIdx;
- (BOOL) isLow:(int) quoteIdx :(int) priceIdx;
- (BOOL) isHigh:(int) quoteIdx :(int) priceIdx;
- (double) getRate:(int) quoteIdx :(int) priceIdx;
- (double) getAmount:(int) quoteIdx :(int) priceIdx;
- (NSString*) getCondition:(int) quoteIdx :(int) priceIdx;
- (NSString*) getOriginator:(int) quoteIdx :(int) priceIdx;

@end
