
@protocol IO2GTableColumnCollection;
@protocol IO2GValueVariant;

@protocol IO2GRow <IAddRef>
    /** Gets the cell value.*/
    - (const void*) getCell: (int) column;
    
    /** Gets the cell value as IO2GValueVariant.*/
    - (NSObject<IO2GValueVariant>*) getCellAsVariant: (int) column;

    /** Is cell changed?*/
    - (BOOL) isCellChanged: (int) column;

    /** Gets columns */
    - (NSObject<IO2GTableColumnCollection>*) columns;

    /** Gets table type */
    - (O2GTable) getTableType;
@end
