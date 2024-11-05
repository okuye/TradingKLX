
@protocol IO2GSystemPropertiesReader <IAddRef>
    /** Gets property by the name.
        @return   0  in case the property not found.
    */
    - (NSString*)findProperty :(NSString*) propertyName;

    /** Gets property by the index.*/
    - (NSString*)getProperty : (int) index :(NSString**)value;

    /** Gets property by the index.*/
    - (NSString*)getProperty : (int) index;

    /** Gets a number of properties*/
    - (int) size;
@end
