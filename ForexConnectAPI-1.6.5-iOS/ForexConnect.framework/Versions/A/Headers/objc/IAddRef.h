
@protocol IAddRef
  // the same as retain
  //-(id) addRef;
  // standard memory management
@optional
  -(id) retain;
  -(oneway void) release;
  -(id) autorelease; 
@end

typedef double DATE;
