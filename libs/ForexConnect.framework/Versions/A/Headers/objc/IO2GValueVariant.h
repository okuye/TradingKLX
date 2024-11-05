#import <Foundation/Foundation.h>
#import "IO2GColumn.h"

@protocol IO2GValueVariant <IAddRef>

- (instancetype)initWithCString:(const char*)data;
- (instancetype)initWithString:(NSString*)data;
- (instancetype)initWithDouble:(double)data;
- (instancetype)initWithInteger:(int)data;
- (instancetype)initWithDate:(double)data;
- (instancetype)initWithBoolean:(BOOL)data;

- (O2GTableColumnType) getType;

- (BOOL)getBoolean;
- (NSString*)getString;
- (double)getDate;
- (double)getDouble;
- (int)getInteger;

@end

@interface O2GValueVariant : NSObject<IO2GValueVariant> {
    O2GTableColumnType _cellType;
    void *_data;
}

- (instancetype)initWithCString:(const char*)data;
- (instancetype)initWithString:(NSString*)data;
- (instancetype)initWithDouble:(double)data;
- (instancetype)initWithInteger:(int)data;
- (instancetype)initWithDate:(double)data;
- (instancetype)initWithBoolean:(BOOL)data;

- (const void*)getUnsafeUnmutablePointer;

- (O2GTableColumnType) getType;

- (BOOL)getBoolean;
- (NSString*)getString;
- (double)getDate;
- (double)getDouble;
- (int)getInteger;

@end
