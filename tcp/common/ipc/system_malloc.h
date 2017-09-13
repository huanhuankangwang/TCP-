#ifndef _SYSTEM_MALLOC_H_
#define _SYSTEM_MALLOC_H_

#include <system_typedef.h>

void * WTMalloc( unsigned int nMemorySize );
void* WTCalloc(unsigned int nElements, unsigned int nElementSize);
void* WTRealloc( void * pvAddr,unsigned int uSize );
WT_Error_Code WTFree( void * pvAddr );

#endif//_SYSTEM_MALLOC_H_