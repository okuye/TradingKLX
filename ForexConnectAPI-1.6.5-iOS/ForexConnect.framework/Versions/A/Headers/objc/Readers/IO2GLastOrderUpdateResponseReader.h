@protocol IO2GLastOrderUpdateResponseReader <IAddRef>

    /** Get order update type. */
    - (O2GTableUpdateType) getUpdateType;

    /** Get order row. */
    - (NSObject<IO2GOrderRow>*)getOrder;

@end
