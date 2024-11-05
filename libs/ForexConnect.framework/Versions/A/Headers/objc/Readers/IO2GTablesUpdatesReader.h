@protocol IO2GTablesUpdatesReader <IAddRef>
    /** Gets current server timestamp.
        @return     Server Timestamp
    */
    - (DATE) getServerTime;
    /** Gets updates size.*/
    - (int) size;
    /** Gets update type.*/
    - (O2GTableUpdateType) getUpdateType : (int) index;
    /** Gets update table*/
    - (O2GTable) getUpdateTable : (int) index;
    /** Gets offer row by index.
        @return     Offer row. If the specified row is not offer then
                    return NULL.
    */
    - (NSObject<IO2GOfferRow>*) getOfferRow : (int) index;
    /** Get account row.
        @return         Account row .If the specified row is not account then
                        return NULL.
    */
    - (NSObject<IO2GAccountRow>*) getAccountRow : (int) index;
    /** Get account row.
        @return         Order row. If the specified row is not order then
                        return NULL.
    */
    - (NSObject<IO2GOrderRow>*) getOrderRow : (int) index;
    /** Get trade row.
        @return         Trade row. If the specified row is not order then
                        return NULL.
    */
    - (NSObject<IO2GTradeRow>*) getTradeRow : (int) index;
    /** Get closed trade row.
        @return         Closed trade row. If the specified row is not order then
                        return NULL.
    */
    - (NSObject<IO2GClosedTradeRow>*) getClosedTradeRow : (int) index;
    /** Get message row.
        @return         Message row. If the specified row is not order then
                        return NULL.
    */
    - (NSObject<IO2GMessageRow>*) getMessageRow : (int) index;
@end
