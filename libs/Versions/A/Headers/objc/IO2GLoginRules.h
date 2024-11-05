
/** Provide information about loaded table.*/
@protocol IO2GLoginRules <IAddRef>
    /** Check loading table during login.*/
    - (BOOL) isTableLoadedByDefault: (O2GTable) table; 
    /** Get response for loaded table.*/
    - (NSObject<IO2GResponse>*) getTableRefreshResponse: (O2GTable) table; 
    /** Gets system properties.*/
    - (NSObject<IO2GResponse>*) getSystemPropertiesResponse;
    /** Gets permission checker. */
    - (NSObject<IO2GPermissionChecker>*) getPermissionChecker;
    /** Gets trading settings provider. */
    - (NSObject<IO2GTradingSettingsProvider>*) getTradingSettingsProvider;
@end


