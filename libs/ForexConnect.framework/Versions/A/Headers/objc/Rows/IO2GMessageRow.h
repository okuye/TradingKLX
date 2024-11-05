@protocol IO2GMessageRow <IO2GRow>
    - (NSString*) getMsgID;
    - (DATE) getTime;
    - (NSString*) getFrom;
    - (NSString*) getType;
    - (NSString*) getFeature;
    - (NSString*) getText;
    - (NSString*) getSubject;
    - (BOOL) getHTMLFragmentFlag;
    //

@end


@protocol IO2GMessageTableRow <IO2GMessageRow>
@end

