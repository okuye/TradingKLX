
typedef enum
{
    IO2GTableColumn_Integer,
    IO2GTableColumn_Double,
    IO2GTableColumn_String,
    IO2GTableColumn_Date,
    IO2GTableColumn_Boolean
}  O2GTableColumnType;

@protocol IO2GTableColumn <IAddRef>
/** Gets the unique identifier of the column.*/
    - (NSString*) getID;
    /** Get the type of the column.*/
    - (O2GTableColumnType) getType;  
@end

@protocol IO2GTableColumnCollection <IAddRef>
    /** Gets a number of the columns.*/
    - (int) size;
    /** Gets the column by the index.*/
    - (NSObject<IO2GTableColumn>*) get: (int) index;
    /** Find the column by the unique identifier.*/
    - (NSObject<IO2GTableColumn>*) find: (NSString *) ID;
@end