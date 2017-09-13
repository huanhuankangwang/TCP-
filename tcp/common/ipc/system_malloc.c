


/*******************************************************************
 **                     Memory definitions                        **
 *******************************************************************/
void * WTMalloc( unsigned int nMemorySize )
{
    PVOID pvMem =  NULL;

    WTASSERT(nMemorySize > 0);

    if(nMemorySize > 0)
    {
        pvMem =  malloc(nMemorySize);
    }

    WTASSERT(pvMem != NULL);
    if(NULL == pvMem)
    {

        WTDEBUG(MODULE_NAME,ERROR_LEVEL, "[CSMalloc]ERROR[errno=%d]: malloc %x fail\r\n", errno,nMemorySize);
    }
    return pvMem;
}

void* WTCalloc(unsigned int nElements, unsigned int nElementSize)
{
    PVOID pvMem =  NULL;

    WTASSERT(nElements > 0);
    WTASSERT(nElementSize > 0);

    if((nElements>0) && (nElementSize>0))
    {
        pvMem =  calloc(nElements,nElementSize);
    }

    WTASSERT(pvMem != NULL);
    if(NULL == pvMem)
    {
        CSDEBUG(MODULE_NAME,ERROR_LEVEL, "[CSCalloc]ERROR[errno=%d]: calloc %x %x fail\r\n", errno,nElements,nElementSize);
    }

    return pvMem;
}

void* WTRealloc( void * pvAddr,unsigned int uSize )
{
    PVOID pvMem =  NULL;

    if(uSize > 0)
    {
        pvMem =  realloc(pvAddr,uSize);
    }

    WTASSERT(pvMem != NULL);

    return pvMem;
}

WT_Error_Code WTFree( void * pvAddr )
{
    if (pvAddr != NULL)
    {
        free(pvAddr);
    }

    return WT_SUCCESS;
}

