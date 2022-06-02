//
//  MemoryMgr.hpp
//  HelloMemory
//
//  Created by stone
//

#ifndef MemoryMgr_hpp
#define MemoryMgr_hpp
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include "CellLog.hpp"
#define MAX_MEMORY_SIZE 64
class MemoryAlloc;
//内存块   内存池中最小单元
class MemoryBlock
{
public:
    MemoryBlock();
    ~MemoryBlock();
public:
    //内存块编号
    int nID;
    //引用次数
    int nRef;
    //所属内存块
    MemoryAlloc* pAlloc;
    //下一块位置
    MemoryBlock* pNext;
    //是否在内存池中
    bool bPool;
};
//内存池
class MemoryAlloc
{
public:
    MemoryAlloc()
    {
        _pBuf=nullptr;
        _pHeader=nullptr;
        _nSize=0;
        _nBlockSize=0;
        //CellLog::info("MemoryAlloc\n");
    }
    void* allocMemory(size_t nSize)
    {
        std::lock_guard<std::mutex> lg(_mutex);
        if(!_pBuf)
        {
            initMemory();
        }
        MemoryBlock* pReturn = nullptr;
        if(nullptr == _pHeader)
        {
            pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
            pReturn->bPool = false;
            pReturn->nRef = 1;
            pReturn->pAlloc = this;
            pReturn->nID = -1;
            pReturn->pNext = nullptr;
            //CellLog::info("allocMem: %lx,id=%d,size=%d\n",pReturn,pReturn->nID,nSize);
        }
        else
        {
            pReturn = _pHeader;
            _pHeader = _pHeader->pNext;
            pReturn->nRef=1;
        }
        //CellLog::info("allocMem: %llx, id=%d, size=%zu\n", pReturn, pReturn->nID, nSize);
        return ((char*)pReturn+sizeof(MemoryBlock));
    }
    void freeMemory(void* pMem)
    {
        MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem-sizeof(MemoryBlock));
        assert(1==pBlock->nRef);
        if(--pBlock->nRef!=0)
        {
            return;
        }
        if(pBlock->bPool)
        {
            std::lock_guard<std::mutex> lg(_mutex);
            pBlock->pNext = _pHeader;
            pBlock->nRef--;
            _pHeader = pBlock;
        }
        else
        {
            free(pBlock);
        }
    }
    //初始化
    void initMemory()
    {
        //断言
        assert(nullptr == _pBuf);
        if(_pBuf)
            return;
        size_t realSize = _nSize+sizeof(MemoryBlock);
        //计算内存池的大小
        size_t bufSize = realSize*_nBlockSize;
        //向系统申请池内存
        _pBuf = (char*)malloc(bufSize);
        //初始化内存池
        _pHeader = (MemoryBlock*)_pBuf;
        _pHeader->bPool = true;
        _pHeader->nRef = 0;
        _pHeader->pAlloc = this;
        _pHeader->nID = 0;
        _pHeader->pNext = nullptr;
        
        MemoryBlock* pTemp = _pHeader;
        for(size_t n=1;n<_nBlockSize;n++)
        {
            MemoryBlock* pBlock = (MemoryBlock*)(_pBuf+(n*realSize));
            pBlock->bPool = true;
            pBlock->nRef = 0;
            pBlock->pAlloc = this;
            pBlock->nID = (int)n;
            pTemp->pNext = pBlock;
            pTemp = pTemp->pNext;
            pBlock->pNext = nullptr;
        }
    }
    ~MemoryAlloc()
    {
        if(_pBuf)
            free(_pBuf);
    }
protected:
    //内存池地址
    char* _pBuf;
    //头部内存单元
    MemoryBlock* _pHeader;
    //内存单元的大小
    size_t _nSize;
    //内存单元的数量
    size_t _nBlockSize;
    std::mutex _mutex;
};

template<size_t nSize,size_t nBlockSize>
class MemoryAlloctor : public MemoryAlloc
{
public:
  MemoryAlloctor()
  {
      const size_t n = sizeof(void*);
      _nSize = (nSize/n)*n + (nSize % n ? n:0);
      _nBlockSize = nBlockSize;
  }
};
//内存池管理类
class MemoryMgr
{
private:
    MemoryMgr()
    {
        init_szAlloc(0, 64, &_mem64);
        //init_szAlloc(65, 128, &_mem128);
        //init_szAlloc(129, 256, &_mem256);
        //init_szAlloc(257, 512, &_mem512);
        //init_szAlloc(513, 1024, &_mem1024);
        //CellLog::info("MemoryMgr\n");
    };
public:
    static MemoryMgr& getInstance();
    void* allocMem(size_t nSize);
    void freeMem(void* p);
    //增加内存引用计数
    void addRef(void* pMem)
    {
        MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem-sizeof(MemoryBlock));
        ++pBlock->nRef;
    };
    ~MemoryMgr();
private:
    //初始化内存池映射数组
    void init_szAlloc(int nBegin,int nEnd,MemoryAlloc* pMem)
    {
        for(int n=nBegin;n<=nEnd;n++)
        {
            _szAlloc[n]=pMem;
        }
    }
private:
    MemoryAlloctor<64, 100000> _mem64;
    //MemoryAlloctor<128, 100000> _mem128;
//    MemoryAlloctor<256, 100000> _mem256;
//    MemoryAlloctor<512, 100000> _mem512;
//    MemoryAlloctor<1024, 100000> _mem1024;
    MemoryAlloc* _szAlloc[MAX_MEMORY_SIZE+1];
};

#endif /* MemoryMgr_hpp */
