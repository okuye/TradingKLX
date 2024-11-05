#import "Rows/IO2GAccountRow.h"
#import "Rows/IO2GOfferRow.h"
#import "Rows/IO2GOrderRow.h"
#import "Rows/IO2GClosedTradeRow.h"
#import "Rows/IO2GTradeRow.h"
#import "Rows/IO2GMessageRow.h"
#import "Rows/IO2GSummaryRow.h"
#import "IO2GValueVariant.h"

@protocol IO2GOffersTable;

/** Generic table response reader.*/
@protocol IO2GGenericTableResponseReader <IAddRef>
    /** Gets a number of the rows in the table.*/
    - (int) size;
    /** Gets the column of the table.*/
    - (NSObject<IO2GTableColumnCollection>*) columns;
    /** Gets the cell value.*/
    - (const void*) getCell: (int) row :(int) column;
    /** Check cell validation.*/
    - (BOOL) isCellValid: (int) row :(int) column;
    /** Gets table type.*/
    - (O2GTable) getType;
    /** Gets generic row.*/
    - (NSObject<IO2GRow>*)getGenericRow :(int) rowIndex;
@end

/** Offers table response reader */
@protocol IO2GOffersTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GOfferRow>*) getRow: (int)index;
@end

/** Accounts table response reader */
@protocol IO2GAccountsTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GAccountRow>*) getRow: (int)index;

@end

/** Orders table response reader */
@protocol IO2GOrdersTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GOrderRow>*) getRow: (int)index;

@end

/** Trades table response reader */
@protocol IO2GTradesTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GTradeRow>*) getRow: (int)index;
@end

/** Closed trades table response reader */
@protocol IO2GClosedTradesTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GClosedTradeRow>*) getRow: (int)index;
@end

/** Messages table response reader */
@protocol IO2GMessagesTableResponseReader <IO2GGenericTableResponseReader>
    - (NSObject<IO2GMessageRow>*) getRow: (int)index;
@end

/** Protocol of the table rows iterator.*/
@protocol IO2GTableIterator <NSObject, IAddRef>
-(void)reset;
@end

/** Interface of the table rows iterator.*/
@interface O2GTableIterator : NSObject<IO2GTableIterator>
@end

/** Table events listener.*/
@protocol IO2GTableListener <IAddRef>
    - (void) onAdded: (NSString*) rowID :(id<IO2GRow>) rowData;
    - (void) onChanged: (NSString*) rowID :(id<IO2GRow>) rowData;
    - (void) onDeleted: (NSString*) rowID :(id<IO2GRow>) rowData;

    - (void) onStatusChanged: (O2GTableStatus) status;
@end

@protocol IO2GEachRowListener <IAddRef>
    - (void) onEachRow: (NSString*) rowID :(id<IO2GRow>) rowData;
@end

/** Generic table interface.*/
@protocol IO2GTable <IO2GGenericTableResponseReader>
    /** Subscribe to specific Update event.*/
    - (void) subscribeUpdate: (O2GTableUpdateType) updateType :(id<IO2GTableListener>) listener;
    /** Unsubscribe from specific Update event.*/
    - (void) unsubscribeUpdate: (O2GTableUpdateType) updateType :(id<IO2GTableListener>) listener;

    /** Subscribe to Status event.*/
    - (void) subscribeStatus: (id<IO2GTableListener>) listener;
    /** Unsubscribe from Status event.*/
    - (void) unsubscribeStatus: (id<IO2GTableListener>) listener;

    /** Get current table status.*/
    - (O2GTableStatus) getStatus;

    /** For each row.*/
    - (void) forEachRow: (id<IO2GEachRowListener>) listener;    

    /** Is cell changed?*/
    - (BOOL) isCellChanged: (int)row :(int) column;

    /** Get next generic row by iteration.*/
    - (BOOL) getNextGenericRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GRow> __autoreleasing *) row;

    /**
     getNextGenericRowBy* functions family
    
     Sample:

     id<IO2GRow> row;
     IO2GTableIterator ordersIterator = nil;
     NSArray *ops = @[[NSNumber numberWithInt:EqualTo]];
     NSArray *names = @[@"Instrument", @"Instrument"];
     O2GValueVariant *variant1 = [[O2GValueVariant alloc] initWithString:@"EUR/USD"];
     O2GValueVariant *variant2 = [[O2GValueVariant alloc] initWithString:@"GBP/USD"];
     NSArray *values = [NSArray arrayWithObjects:variant1, variant2, nil];

     [genericTableObject getNextGenericRowByMultiColumnValues:names :ops :values :OperatorOR :&ordersIterator :&row];
     **/

    /** Get next generic row by multi column.*/
    - (BOOL) getNextGenericRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextGenericRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GRow> __autoreleasing *) row;

    /** Get next generic row by column values.*/
    - (BOOL) getNextGenericRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GRow> __autoreleasing *)row;

    /** Get next generic row by condition value.*/
    - (BOOL) getNextGenericRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GRow> __autoreleasing *)row;

    /**
     **/

    /** Enumerate all generic row by specific column.*/
    - (void)enumerateRowByColumnValueUsingBlock:(NSString*)columnID :(O2GValueVariant*)columnValueAsVariant :(void (^)(NSObject<IO2GRow>* row, int index, BOOL *stop)) block;

    /** Enumerate all generic row.*/
    - (void)enumerateRowUsingBlock:(void (^)(NSObject<IO2GRow>* row, int index, BOOL *stop)) block;

    /** Get array of all generic row.*/
    - (NSArray*)asSecquence;

    /** Set the type of the table event filter.*/
    - (void) setTableEventsFilter: (O2GTableEventsFilter) tableFilterType;

    /** Get the type of the table event filter.*/
    - (O2GTableEventsFilter) getTableEventsFilter;

    /** Get the offerse table instance */
    - (NSObject<IO2GOffersTable>*)getOffersTable;

@end

typedef enum
{
    O2GUpdatesProcessStatus_UnknowProcessStatus = -1,
    O2GUpdatesProcessStatus_BeginTablesUpdate = 0,
    O2GUpdatesProcessStatus_EndTablesUpdate = 1,
} IO2GTableManager_O2GUpdatesProcessStatus;

/** Updates process event listener.*/
@protocol IO2GUpdatesProcessStatusListener <IAddRef>
    -(void) onUpdatesProcessStatusChanged: (IO2GTableManager_O2GUpdatesProcessStatus) status;
@end

/** Table manager.*/
@protocol IO2GTableManager
    /** Get specific table.*/
    - (NSObject<IO2GTable>*) getTable: (O2GTable) tableType;

    - (O2GTableManagerStatus) getStatus;

    /** Lock all rows.*/
    - (void) lockUpdates;

    /** Unlock all rows.*/
    - (void) unlockUpdates;

    /** Subscribe to status of Update's process event.*/
    -(void) subscribeUpdatesProcessStatus:(id<IO2GUpdatesProcessStatusListener>) listener;

    /** Unsubscribe from status of Update's process event.*/
    - (void) unsubscribeUpdatesProcessStatus:(id<IO2GUpdatesProcessStatusListener>) listener;
	
	/** Get account ID. Returns non empty string from table magers by accounts only. */
	-(NSString*) getAccountID;
@end

/** Offers table.*/
@protocol IO2GOffersTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GOfferTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GOfferTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GOfferTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GOfferTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GOfferTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GOfferTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL)  getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GOfferTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GOfferTableRow> __autoreleasing *)row;
@end

/** Accounts table.*/
@protocol IO2GAccountsTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GAccountTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GAccountTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GAccountTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GAccountTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GAccountTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GAccountTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GAccountTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GAccountTableRow> __autoreleasing *)row;
@end

/** Orders table.*/
@protocol IO2GOrdersTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GOrderTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GOrderTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GOrderTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GOrderTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GOrderTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GOrderTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GOrderTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GOrderTableRow> __autoreleasing *)row;
@end

/** Trades table.*/
@protocol IO2GTradesTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GTradeTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GTradeTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GTradeTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GTradeTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GTradeTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GTradeTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GTradeTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GTradeTableRow> __autoreleasing *)row;
@end

/** Closed trades table.*/
@protocol IO2GClosedTradesTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GClosedTradeTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GClosedTradeTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
- (BOOL) findRow: (NSString*) ID : (id<IO2GClosedTradeTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GClosedTradeTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GClosedTradeTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GClosedTradeTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GClosedTradeTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GClosedTradeTableRow> __autoreleasing *)row;
@end

/** Messages table.*/
@protocol IO2GMessagesTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GMessageTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GMessageTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GMessageTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GMessageTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GMessageTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GMessageTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GMessageTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GMessageTableRow> __autoreleasing *)row;
@end

/** Summary table.*/
@protocol IO2GSummaryTable <IO2GTable>
    /** Get row specified by index.*/
    - (NSObject<IO2GSummaryTableRow>*) getRow: (int)index;

    /** Get next row by iteration.*/
    - (BOOL) getNextRow: (NSObject<IO2GTableIterator>*) iterator :(id<IO2GSummaryTableRow> __autoreleasing *) row ;

    /** Find row specified by ID.*/
    - (BOOL) findRow: (NSString*) ID : (id<IO2GSummaryTableRow> __autoreleasing *) row;

    /** Find row specified by ID.*/
    - (NSObject<IO2GSummaryTableRow>*) findRow: (NSString*) ID;

    /** Get next row by multi column.*/
    - (BOOL) getNextRowByMultiColumnValues:(NSArray<NSString*>*)columnNames :(NSArray<NSNumber*>*)conditions :(NSArray<O2GValueVariant*>*)columnValues :(O2GLogicOperators)logicOperator :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GSummaryTableRow> __autoreleasing *) row;

    /** Get next row by specific column.*/
    - (BOOL) getNextRowByColumnValue:(NSString *) columnID :(O2GValueVariant*) columnValueAsVariant :(NSObject<IO2GTableIterator>*) iterator :(id<IO2GSummaryTableRow> __autoreleasing *) row;

    /** Get next row by column values.*/
    - (BOOL) getNextRowByColumnValues:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GSummaryTableRow> __autoreleasing *)row;

    /** Get next row by condition value.*/
    - (BOOL) getNextRowByCondition:(NSString*)columnName :(O2GRelationalOperators)condition :(NSArray<O2GValueVariant*>*)columnValues :(NSObject<IO2GTableIterator>*)iterator :(id<IO2GSummaryTableRow> __autoreleasing *)row;

@end
