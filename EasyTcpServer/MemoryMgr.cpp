#include "MemoryMgr.hpp"

MemoryMgr::~MemoryMgr()
{
    
}
MemoryMgr& MemoryMgr::getInstance()
{
    static MemoryMgr instance;
    return instance;
}
void* MemoryMgr::allocMem(size_t nSize)
{
    if(nSize<=MAX_MEMORY_SIZE)
    {
        return _szAlloc[nSize]->allocMemory(nSize);
    }
    else
    {
        MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
        pReturn->bPool = false;
        pReturn->nRef = 1;
        pReturn->pAlloc = nullptr;
        pReturn->nID = -1;
        pReturn->pNext = nullptr;
        //CellLog::info("allocMem: %llx, id=%d, size=%zuu\n", pReturn , pReturn->nID, nSize);
        return ((char*)pReturn+sizeof(MemoryBlock));
    }
}
void MemoryMgr::freeMem(void *pMem)
{
    
    MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem-sizeof(MemoryBlock));
    //CellLog::info("freeMem: %llx, id=%d\n", pBlock, pBlock->nID);
    if(pBlock->bPool)
    {
        pBlock->pAlloc->freeMemory(pMem);
    }
    else
    {
        if(--pBlock->nRef == 0)
            free(pBlock);
    }
}
