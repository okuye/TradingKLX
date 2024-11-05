#ifdef WIN32
#define Order2Go2 __declspec(dllimport)
#else
#define Order2Go2
#endif

#ifdef __cplusplus
 #include "interfaces_all.h"
#endif
#ifdef __OBJC__
 #import "objc/interfaces_all.h"
#endif
