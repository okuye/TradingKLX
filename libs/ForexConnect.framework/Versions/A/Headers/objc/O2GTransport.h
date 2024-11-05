@interface O2GTransport : NSObject
{}
    /** Create transport session.*/
    + (NSObject<IO2GSession>*) createSession;
    
    /** Set proxy.
        @param proxyHost            Proxy host.
        @param iPort                Proxy port.
        @param user                 User name.
        @param password             User password.
    */
    + (void) setProxy: (NSString*) proxyHost :(int) port :(NSString*) user :(NSString*) password;

    /** Set CA info (Useless in Windows OS)
        @param caFilePath           File path to certificate (certificate bundle) in PEM format.
    */
    + (void) setCAInfo: (NSString*) caFilePath;

    /** Set number of reconnection tries
    @param iNumber              Number of reconnection tries.
    */
    + (void) setNumberOfReconnections: (const int) number;

    + (void)setApplicationID: (NSString*) applicationID;

    + (NSString*)getApplicationID;

    + (void)setClosedHistorySize: (const int)size;

    + (void)O2GTransport_finalize;

@end
