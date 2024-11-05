#pragma once


typedef enum
{
    O2GRolloverStatus_RolloverNotLoaded,
    O2GRolloverStatus_RolloverLoading,
    O2GRolloverStatus_RolloverReady,
    O2GRolloverStatus_FailToLoad
} IO2GRolloverProvider_O2GRolloverStatus;

@protocol IO2GRolloverProviderListener <IAddRef>
-(void)onStatusChanged:(O2GRolloverStatus)status;
@end

@protocol IO2GRolloverProvider <IAddRef>

- (double)getRolloverBuy:(id<IO2GOfferRow>)offer :(id<IO2GAccountRow>)account;
- (double)getRolloverSell:(id<IO2GOfferRow>)offer :(id<IO2GAccountRow>)account;
- (O2GRolloverStatus)getStatus;
- (void)subscribe:(id<IO2GRolloverProviderListener>)listener;
- (void)unsubscribe:(id<IO2GRolloverProviderListener>)listener;
- (void)refreshRolloverProfiles;

@end
