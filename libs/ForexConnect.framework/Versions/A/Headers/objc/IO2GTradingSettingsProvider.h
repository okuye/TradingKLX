#import <Foundation/Foundation.h>
#import "IO2GProgressiveMarginLevels.h"

/** See system param SHOW_MR. */
typedef NS_ENUM(NSInteger, IO2GTradingSettingsProvider_O2GShowMR)
{
    /* used when no SHOW_MR property received - default behavior */
	O2GShowMR_ShowUndefinedMR,
	O2GShowMR_ShowEMR,
	O2GShowMR_ShowMMR,
	O2GShowMR_ShowLMR,
	O2GShowMR_ShowAllMR,
};


@protocol IO2GTradingSettingsProvider <IAddRef>

    /** Gets condition distance for the stop. */
    - (int) getCondDistStopForTrade: (NSString*) instrument;

    /** Gets condition distance for the limit. */
    - (int) getCondDistLimitForTrade: (NSString*) instrument;

    /** Gets condition distance for the entry stop. */
    - (int) getCondDistEntryStop: (NSString*) instrument;

    /** Gets condition distance for the entry limit. */
    - (int) getCondDistEntryLimit: (NSString*) instrument;

    /** Gets minimum quantity (in lots) for an order for specified instrument and account. */
    - (int) getMinQuantity: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Gets maximum quantity (in lots) for an order for specified instrument and account. */
    - (int) getMaxQuantity: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Gets base unit size by the account identifier and instrument identifier. */
    - (int) getBaseUnitSize: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Gets marker status. */
    - (O2GMarketStatus) getMarketStatus: (NSString*) instrument;

    /** Gets minimal trailing step. */
    - (int) getMinTrailingStep;

    /** Gets maximal trailing step. */
    - (int) getMaxTrailingStep;

    /** Gets the MMR for the specified instrument by account. */
    - (double) getMMR: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Gets the MMR/LMR/EMR for the specified instrument by account.

        @param  instrument      Instrument.
        @param  account         Account row.
        @param  mmr             [out] MMR.
        @param  emr             [out] EMR.
        @param  lmr             [out] LMR.

        @return Whether three level margin is used.
    */
    - (BOOL) getMargins: (NSString*) instrument :(id<IO2GAccountRow>) account :(double*) mmr :(double*) emr :(double*) lmr;

    /** Gets the Progressive margins levels for the specified instrument.
     
        @param  instrument      Instrument.
        @param  account         Account row.

        @return Progressive margins levels. If progressive margins is not used then return nil 
    */
    - (NSObject<IO2GProgressiveMarginLevels>*) getProgressiveMarginLevels: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Detect whether progressive margin is used for specified instrument.
     
        @param  instrument      Instrument.
        @param  account         Account row.

        @return Whether progressive margin is used.
    */
   - (BOOL) isProgressiveMargin: (NSString*) instrument :(id<IO2GAccountRow>) account;

    /** Detect whether three level margin is used for specified account
     
     @param  account         Account row.
     
     @return Whether three level margin is used.
     */
    
    - (BOOL) isTier3:(id<IO2GAccountRow>)account;

    /** Gets a value of SHOW_MR system property as a enum value. */
    - (IO2GTradingSettingsProvider_O2GShowMR) getShowMR;

@end
