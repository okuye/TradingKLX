#pragma once


typedef enum
{
    IO2GCommissionProvider_CommissionStatusDisabled = 0,
    IO2GCommissionProvider_CommissionStatusLoading = 1,
    IO2GCommissionProvider_CommissionStatusReady = 2,
    IO2GCommissionProvider_CommissionStatusFailToLoad = 3
} IO2GCommissionProvider_O2GCommissionStatus;

@protocol IO2GCommissionDescription <IAddRef>
    -(O2GCommissionStage) getStage;
    - (O2GCommissionUnitType) getUnitType;
    - (double) getCommissionValue;
    - (double) getMinCommission;
@end


@protocol IO2GCommissionDescriptionsCollection <IAddRef>
    - (int)size;
    - (NSObject<IO2GCommissionDescription>*) getByIndex:(int)i;
@end

@protocol IO2GCommissionProviderListener <IAddRef>
    -(void)onChangeCommissionProviderStatus:(O2GCommissionStatus)status;
    -(void)onNeedUpdateCommissions;
@end


@protocol IO2GCommissionsProvider <IAddRef>
    
    /** Gets condition distance for the stop. */
    - (NSObject<IO2GCommissionDescriptionsCollection>*)getCommissionDescriptions:(NSString*)offerID :(NSString*)atpID;
    
    /** Gets the current status. */
    - (O2GCommissionStatus)getStatus;
    - (void)subscribe:(id<IO2GCommissionProviderListener>)listener;
    - (void)unsubscribe:(id<IO2GCommissionProviderListener>)listener;
    
    //Methoods to calculte commissions
    - (double) calcOpenCommission:(id<IO2GOfferTableRow>)offer :(id<IO2GAccountTableRow>)account :(int)amount :(NSString*)buySell :(double)rate;
    - (double) calcCloseCommission:(id<IO2GOfferTableRow>)offer :(id<IO2GAccountTableRow>)account :(int)amount :(NSString*)buySell :(double)rate;
    - (double) calcTotalCommission:(id<IO2GOfferTableRow>)offer :(id<IO2GAccountTableRow>)account :(int)amount :(NSString*)buySell :(double)rateOpen :(double)rateClose;

@end
