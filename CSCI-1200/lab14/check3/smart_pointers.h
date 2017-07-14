#include <cstdlib>

// ==============================================================================

template <class T> class dsAutoPtr 
{
public:
    explicit dsAutoPtr(T* p = NULL) : ptr(p) {}
    ~dsAutoPtr()     { delete ptr; }
    T& operator*()   { return *ptr; }
    T* operator->()  { return ptr; }

private:
    T* ptr;
};


// ==============================================================================

class ReferenceCount 
{
public:
    void addReference() { count++; }
    int releaseReference() { return --count; }

private:
    int count; 
};

// ==============================================================================

template <class T> class dsSharedPtr 
{
public:
    dsSharedPtr(T* pValue = NULL) : pData(pValue), reference(0) {
        reference = new ReferenceCount();
        reference->addReference();
    }

    dsSharedPtr(const dsSharedPtr<T>& sp) : pData(sp.pData), reference(sp.reference) {
        reference->addReference();
    }

    dsSharedPtr<T>& operator=(const dsSharedPtr<T>& sp) {
        if (this != &sp) {
            if(reference->releaseReference() == 0) {
                delete pData;
                delete reference;
            }
            pData = sp.pData;
            reference = sp.reference;
            reference->addReference();
        }
        return *this;
    }

    ~dsSharedPtr() {
        if (reference->releaseReference() == 0) {
            delete pData;
            delete reference;
        }
    }

    bool operator==(const dsSharedPtr<T>& sp) { return pData == sp.pData; }
    T& operator*()  { return *pData; }
    T* operator->() { return pData; }

private:
    T* pData;  
    ReferenceCount* reference; 
};

// ==============================================================================