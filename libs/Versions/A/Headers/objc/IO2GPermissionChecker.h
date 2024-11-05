
/** Permission checker.*/
@protocol IO2GPermissionChecker <IAddRef>

    /** Whether the open market order can be created. */
    - (O2GPermissionStatus) canCreateMarketOpenOrder: (NSString*) instrument;

    /** Whether the open market order can be changed. */
    - (O2GPermissionStatus) canChangeMarketOpenOrder: (NSString*) instrument;

    /** Whether the open market order can be deleted. */
    - (O2GPermissionStatus) canDeleteMarketOpenOrder: (NSString*) instrument;

    /** Whether the close market order can be created. */
    - (O2GPermissionStatus) canCreateMarketCloseOrder: (NSString*) instrument;

    /** Whether the close market order can be changed. */
    - (O2GPermissionStatus) canChangeMarketCloseOrder: (NSString*) instrument;

    /** Whether the close market order can be deleted. */
    - (O2GPermissionStatus) canDeleteMarketCloseOrder: (NSString*) instrument;

    /** Whether the open entry order can be created. */
    - (O2GPermissionStatus) canCreateEntryOrder: (NSString*) instrument;

    /** Whether the open entry order can be changed. */
    - (O2GPermissionStatus) canChangeEntryOrder: (NSString*) instrument;

    /** Whether the open entry order can be deleted. */
    - (O2GPermissionStatus) canDeleteEntryOrder: (NSString*) instrument;

    /** Whether the open entry order can be created. */
    - (O2GPermissionStatus) canCreateStopLimitOrder: (NSString*) instrument;

    /** Whether the open entry order can be changed. */
    - (O2GPermissionStatus) canChangeStopLimitOrder: (NSString*) instrument;

    /** Whether the open entry order can be deleted. */
    - (O2GPermissionStatus) canDeleteStopLimitOrder: (NSString*) instrument;

    /** Whether it is possible to request a quote. */
    - (O2GPermissionStatus) canRequestQuote: (NSString*) instrument;

    /** Whether it is possible to accept the quote provided by the dealer. */
    - (O2GPermissionStatus) canAcceptQuote: (NSString*) instrument;

    /** Whether it is possible to delete the quote provided by the dealer. */
    - (O2GPermissionStatus) canDeleteQuote: (NSString*) instrument;

    /** Whether it is possible to create the new OCO. */
    - (O2GPermissionStatus) canCreateOCO: (NSString*) instrument;

    /** Whether it is possible to create the new OTO. */
    - (O2GPermissionStatus) canCreateOTO: (NSString*) instrument;

    /** Whether it is possible to add new orders to the existing ContingencyGroup or creating new. */
    - (O2GPermissionStatus) canJoinToNewContingencyGroup: (NSString*) instrument;

    /** Whether it is possible to add new orders to the existing ContingencyGroup or creating new. */
    - (O2GPermissionStatus) canJoinToExistingContingencyGroup: (NSString*) instrument;

    /** Whether it is possible to exclude orders from the existing ContingencyGroup. */
    - (O2GPermissionStatus) canRemoveFromContingencyGroup: (NSString*) instrument;

    /** Whether it is possible to change the offers subscription. */
    - (O2GPermissionStatus) canChangeOfferSubscription: (NSString*) instrument;

    /** Determines whether the user may create order for close all positions for an instrument on an account. */
    - (O2GPermissionStatus) canCreateNetCloseOrder: (NSString*) instrument;

    /** Determines whether the user may change order for close all positions for an instrument on an account. */
    - (O2GPermissionStatus) canChangeNetCloseOrder: (NSString*) instrument;

    /** Determines whether the user may delete order for close all positions for an instrument on an account. */
    - (O2GPermissionStatus) canDeleteNetCloseOrder: (NSString*) instrument;

    /** Determines whether the user may create net stop/limit order. */
    - (O2GPermissionStatus) canCreateNetStopLimitOrder: (NSString*) instrument;

    /** Determines whether the user may change net stop/limit order. */
    - (O2GPermissionStatus) canChangeNetStopLimitOrder: (NSString*) instrument;

    /** Determines whether the user may delete net stop/limit order. */
    - (O2GPermissionStatus) canDeleteNetStopLimitOrder: (NSString*) instrument;

    /** Determines whether the trailing stop is dymanic */
    - (O2GPermissionStatus) canUseDynamicTrailingForStop;

    /** Determines whether the trailing limit is dymanic */
    - (O2GPermissionStatus) canUseDynamicTrailingForLimit;

    /** Determines whether the trailing entry stop is dymanic */
    - (O2GPermissionStatus) canUseDynamicTrailingForEntryStop;

    /** Determines whether the trailing entry limit is dymanic */
    - (O2GPermissionStatus) canUseDynamicTrailingForEntryLimit;

    /** Determines whether the trailing stop is fluctuate */
    - (O2GPermissionStatus) canUseFluctuateTrailingForStop;

    /** Determines whether the trailing limit is fluctuate */
    - (O2GPermissionStatus) canUseFluctuateTrailingForLimit;

    /** Determines whether the trailing entry stop is fluctuate */
    - (O2GPermissionStatus) canUseFluctuateTrailingForEntryStop;

    /** Determines whether the trailing entry limit is fluctuate */
    - (O2GPermissionStatus) canUseFluctuateTrailingForEntryLimit;
@end
