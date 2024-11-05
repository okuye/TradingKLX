@protocol IO2GOrderResponseReader <IAddRef>
    - (NSString*) getOrderID;
    - (BOOL) isUnderDealerIntervention;
@end
