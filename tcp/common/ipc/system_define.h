#ifndef _SYSTEM_DEFINE_H_
#define _SYSTEM_DEFINE_H_

typedef         int                  WT_Error_Code;
typedef         void*                WT_HANDLE;

#define         WTDEBUG(...)
#define         WT_UNUSED(x) ((void)x)

#define WTASSERT(expression)  \
                    do{ \
                        if(!(expression)) \
                        WTDEBUG("WTDEBUG",FATAL_LEVEL,"Assertion: \"%s\" failed, in file %s, line %d\n", \
                                #expression, __FILE__, __LINE__); \
                    }while(0)


#define         WT_NULL             ((WT_Error_Code)0)
#define         WT_SUCCESS          ((WT_Error_Code)1)
#define         WT_FAILURE          ((WT_Error_Code)0)


#endif//_SYSTEM_DEFINE_H_
