#ifndef objectPoolBase_hpp
#define objectPoolBase_hpp

#include <stdlib.h>
#include <mutex>
#include <cassert>
template<class Type ,size_t nPoolSize>
class ObjectPool
{
public:
    ObjectPool()
    {
        _pBuf=nullptr;
        initPool();
    }
    ~ObjectPool(){
        
    };
private:
    class NodeHeader
    {
    public:
        NodeHeader* pNext;
        int nID;
        char nRef;
        bool bPool;
    };
public:
    //初始化对象池
    void initPool()
    {
        assert(nullptr==_pBuf);
        if(_pBuf)
            return;
        size_t realSize = sizeof(Type)+sizeof(NodeHeader);
        size_t n = nPoolSize*(sizeof(Type)+sizeof(NodeHeader));
        //申请池内存
        _pBuf = new char[n];
        //初始化内存池
        _pHeader = (NodeHeader*)_pBuf;
        _pHeader->bPool = true;
        _pHeader->nID = 0;
        _pHeader->nRef = 0;
        _pHeader->pNext = nullptr;
        //遍历初始化
        NodeHeader* temp = _pHeader;
        for(size_t n=1;n<nPoolSize;n++)
        {
            NodeHeader* temp2 = (NodeHeader*)(_pBuf+(n*realSize));
            temp2->bPool = true;
            temp2->nID = n;
            temp2->nRef = 0;
            temp->pNext = temp2;
            temp = temp2;
        }
        
    }
    //申请对象
    void* allocObjMemory(size_t nSize)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        NodeHeader* pReturn = nullptr;
        if(nullptr == _pHeader)
        {
            pReturn = (NodeHeader*)new char[sizeof(Type)+sizeof(NodeHeader)];
            pReturn->bPool = false;
            pReturn->nID = -1;
            pReturn->nRef = 1;
            pReturn->pNext = nullptr;
        }
        else
        {
            pReturn = _pHeader;
            _pHeader=_pHeader->pNext;
            assert((0==pReturn->nRef));
            pReturn->nRef = 1;
        }
       // CellLog::info("allocObjmemory:%lx,ID:%d,size=%d\n",pReturn,pReturn->nID,nSize);
        return ((char*)pReturn+sizeof(NodeHeader));
    }
    //释放对象
    void freeObjMemory(void* pMem)
    {
        NodeHeader* pNode = (NodeHeader*)((char*)pMem-sizeof(NodeHeader));
        assert(1==pNode->nRef);
        std::lock_guard<std::mutex> lock(_mutex);
        if(pNode->bPool)
        {
            if(--pNode->nRef!=0)
            {
                
                return;
            }
            pNode->pNext = _pHeader;
            _pHeader = pNode;
        }
        else
        {
            if(--pNode->nRef!=0)
            {
                return;
            }
            delete [] pNode;
        }
    }
private:
    NodeHeader* _pHeader;
    //对象池内存缓存区地址
    char* _pBuf;
    std::mutex _mutex;
};

template<class Type,size_t nPoolSize>
class ObjectPoolBase
{
public:
    void* operator new(size_t nSize)
    {
        return objectPool().allocObjMemory(nSize);
    }
    void operator delete(void* p)
    {
        free(p);
    }
    template<typename ...Args>
    static Type* createObject(Args... args)
    {
        return new Type(args...);
    }
    static void destroyObject(Type* p)
    {
        delete p;
    }
    ~ObjectPoolBase()
    {
    }
private:
    typedef ObjectPool<Type, nPoolSize> classTypePool;
    static classTypePool& objectPool()
    {
        static classTypePool pool;
        return pool;
    }
};
#endif /* objectPoolBase_hpp */
