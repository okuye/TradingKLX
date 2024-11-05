@protocol IO2GProgressiveMarginLevels <IAddRef>

    /** Get From amount thresholds to specific level */
    -(NSInteger) getFrom:(NSInteger)index;

    /** Get Till (including) amount thresholds to specific level
    Last level defined Till as -1 – meaning infinitive.*/
    -(NSInteger)  getTill:(NSInteger)index;

    /** Get Margin Per Contract*/
    -(double) getMarginPerContract:(NSInteger)index;

    /** Levels count.*/
    -(NSInteger) count;
@end

